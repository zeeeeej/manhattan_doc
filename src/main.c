// Copyright 2022 Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <getopt.h>

//#include "audio.h"
#include "common.h"
#include "isp.h"
#include "log.h"
#include "network.h"
#include "param.h"
#include "rockiva.h"
//#include "server.h"
#include "storage.h"
#include "system.h"
//#include "video.h"
#include "photo.h"
#include <linux/input.h>

#include "uart.h"
#include "data.h"
#include "mpu6887p.h"
#include "heat.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/statvfs.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "rkipc.c"

//enum { LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG };

//int enable_minilog = 0;
//int rkipc_log_level = LOG_INFO;

static int g_main_run_ = 1;
char *rkipc_ini_path_ = NULL;
char *rkipc_iq_file_path_ = NULL;

static void sig_proc(int signo) {
	LOG_INFO("received signo %d \n", signo);
	g_main_run_ = 0;
}

// 获取磁盘空间信息（单位：字节）
static void get_disk_space(const char* path, uint64_t* total, uint64_t* free) {
    struct statvfs stat;

    if (statvfs(path, &stat) == 0) {
        *total = (uint64_t)stat.f_blocks * stat.f_frsize;
        *free = (uint64_t)stat.f_bfree * stat.f_frsize;
    } else {
        *total = *free = 0;
    }
}

static void debug(){
	printf("==============app_info================\n");
	FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo) {
        char line[256];
        while (fgets(line, sizeof(line), cpuinfo)) {
            if (strstr(line, "model name")) {
                printf("CPU: %s", strchr(line, ':') + 2);
                break;
            }
        }
        fclose(cpuinfo);
    }
	// 获取磁盘空间
    uint64_t disk_total, disk_free;
    get_disk_space("/", &disk_total, &disk_free); // Linux/macOS用"/"，Windows可以用"C:"
    
    printf("磁盘空间信息:\n");
    printf("总空间: %.2f GB\n", (double)disk_total / (1024 * 1024 * 1024));
    printf("可用空间: %.2f GB\n", (double)disk_free / (1024 * 1024 * 1024));
    
    // 获取内存信息
    uint64_t mem_total, mem_free;
    get_memory_info(&mem_total, &mem_free);
    
    printf("\n内存信息:\n");
    printf("总内存: %.2f GB\n", (double)mem_total / (1024 * 1024 * 1024));
    printf("可用内存: %.2f GB\n", (double)mem_free / (1024 * 1024 * 1024));
	printf("==============app_info================\n");
}

static const char short_options[] = "c:a:l:";
static const struct option long_options[] = {{"config", required_argument, NULL, 'c'},
                                             {"aiq_file", no_argument, NULL, 'a'},
                                             {"log_level", no_argument, NULL, 'l'},
                                             {"help", no_argument, NULL, 'h'},
                                             {0, 0, 0, 0}};

static void usage_tip(FILE *fp, int argc, char **argv) {
	fprintf(fp,
	        "Usage: %s [options]\n"
	        "Version %s\n"
	        "Options:\n"
	        "-c | --config      rkipc ini file, default is "
	        "/userdata/rkipc.ini, need to be writable\n"
	        "-a | --aiq_file    aiq file dir path, default is /etc/iqfiles\n"
	        "-l | --log_level   log_level [0/1/2/3], default is 2\n"
	        "-h | --help        for help \n\n"
	        "\n",
	        argv[0], "V1.0");
}

void rkipc_get_opt(int argc, char *argv[]) {
	for (;;) {
		int idx;
		int c;
		c = getopt_long(argc, argv, short_options, long_options, &idx);
		if (-1 == c)
			break;
		switch (c) {
		case 0: /* getopt_long() flag */
			break;
		case 'c':
			rkipc_ini_path_ = optarg;
			break;
		case 'a':
			rkipc_iq_file_path_ = optarg;
			break;
		case 'l':
			rkipc_log_level = atoi(optarg);
			break;
		case 'h':
			usage_tip(stdout, argc, argv);
			exit(EXIT_SUCCESS);
		default:
			usage_tip(stderr, argc, argv);
			exit(EXIT_FAILURE);
		}
	}
}
#if 0
#define AO_FREAD_SIZE 1024 * 4
static void *wait_key_event(void *arg) {
	int key_fd;
	key_fd = open("/dev/input/event0", O_RDONLY);
	if (key_fd < 0) {
		LOG_ERROR("can't open /dev/input/event0\n");
		return NULL;
	}
	fd_set rfds;
	int nfds = key_fd + 1;
	struct timeval timeout;
	struct input_event key_event;

	while (g_main_run_) {
		// The rfds collection must be emptied every time,
		// otherwise the descriptor changes cannot be detected
		timeout.tv_sec = 1;
		FD_ZERO(&rfds);
		FD_SET(key_fd, &rfds);
		select(nfds, &rfds, NULL, NULL, &timeout);
		// wait for the key event to occur
		if (FD_ISSET(key_fd, &rfds)) {
			read(key_fd, &key_event, sizeof(key_event));
			LOG_INFO("[timeval:sec:%d,usec:%d,type:%d,code:%d,value:%d]\n", key_event.time.tv_sec,
			         key_event.time.tv_usec, key_event.type, key_event.code, key_event.value);
			if ((key_event.code == KEY_VOLUMEDOWN) && key_event.value) {
				LOG_INFO("get KEY_VOLUMEDOWN\n");
				rkipc_ao_init();
				FILE *fp = fopen("/oem/usr/share/speaker_test.wav", "rb");
				int size = AO_FREAD_SIZE;
				char *tmp_data;
				tmp_data = malloc(AO_FREAD_SIZE);
				while (size > 0) {
					memset((void *)tmp_data, 0, AO_FREAD_SIZE);
					size = fread(tmp_data, 1, AO_FREAD_SIZE, fp);
					rkipc_ao_write(tmp_data, AO_FREAD_SIZE);
				}
				rkipc_ao_write(tmp_data, 0);
				free(tmp_data);
				fclose(fp);
				rkipc_ao_deinit();
			}

			if ((key_event.code == KEY_VOLUMEUP) && key_event.value) {
				LOG_INFO("get KEY_VOLUMEUP\n");
			}
		}
	}

	if (key_fd) {
		close(key_fd);
		key_fd = 0;
	}
	LOG_DEBUG("wait key event out\n");
	return NULL;
}
#endif
int main(int argc, char **argv) {
	//pthread_t key_chk;
	const char* path = "/userdata/jpeg";
	LOG_DEBUG("main begin\n");
	rkipc_version_dump();
	signal(SIGINT, sig_proc);
	signal(SIGTERM, sig_proc);

	rkipc_get_opt(argc, argv);
	LOG_INFO("rkipc_ini_path_ is %s, rkipc_iq_file_path_ is %s, rkipc_log_level "
	         "is %d~~~~~~~~~~~~\n",
	         rkipc_ini_path_, rkipc_iq_file_path_, rkipc_log_level);
	
    if(access(path, F_OK) == 0) {
        LOG_INFO("Directory exists.\n");
    } else {
        if( mkdir(path, 0755) == -1 ){
			LOG_ERROR("create folder fail\n");
		}
    }

	if(access("/userdata/update_ota.tar", F_OK) == 0)
	{
		if(remove("/userdata/update_ota.tar") == 0)
		{
			LOG_INFO("remove ota file success!\n");
		}else{
			LOG_ERROR("remove ota file failed!\n");
		}
	}

	// init
	rk_param_init(rkipc_ini_path_);
	//rk_network_init(NULL);
	//rk_system_init();
	/*if (rk_param_get_int("video.source:enable_npu", 0))
		rkipc_rockiva_init();
	if (rk_param_get_int("video.source:enable_aiq", 1)) {
		rk_isp_init(0, rkipc_iq_file_path_);
		rk_isp_set_frame_rate(0, rk_param_get_int("isp.0.adjustment:fps", 30));
		if (rk_param_get_int("isp:init_form_ini", 1))
			rk_isp_set_from_ini(0);
	}*/
	rk_isp_init(0, rkipc_iq_file_path_);
	rk_isp_set_from_ini(1);
	RK_MPI_SYS_Init();
	//rk_video_init();
	//if (rk_param_get_int("audio.0:enable", 0))
	//	rkipc_audio_init();
	//rkipc_server_init();
	//rk_storage_init();
	//pthread_create(&key_chk, NULL, wait_key_event, NULL);
	//pthread_sem_init();
	qjy_uart_init(qjy_uart_parser);
	gsensor_init();
	qjy_photo_init();
	heat_pwm_init();
	/*sleep(2);
	LOG_INFO("~~%d, %s~~\n", rk_param_get_int("qjy.1:address", 1), rk_param_get_string("qjy.1:serial_num", NULL));
	sleep(2);
	rk_param_set_int("qjy.1:address", 0x10);
    rk_param_set_string("qjy.1:serial_num", "aaaa");
	rk_param_save();
	sleep(2);
	LOG_INFO("~~%d, %s~~\n", rk_param_get_int("qjy.1:address", 1), rk_param_get_string("qjy.1:serial_num", NULL));
	sleep(2);
	qjy_take_photo(1);*/
	while (g_main_run_) {
		usleep(1000 * 1000);
	}

	// deinit
	//pthread_join(key_chk, NULL);
	//rk_storage_deinit();
	//rkipc_server_deinit();
	rk_param_deinit();
	qjy_photo_deinit();
	//rk_system_deinit();
	//rk_video_deinit();
	//if (rk_param_get_int("video.source:enable_aiq", 1))
	rk_isp_deinit(0);
	//if (rk_param_get_int("audio.0:enable", 0))
	//	rkipc_audio_deinit();
	RK_MPI_SYS_Exit();
	//if (rk_param_get_int("video.source:enable_npu", 0))
	//	rkipc_rockiva_deinit();
	//rk_network_deinit();
	pthread_sem_deinit();
	qjy_uart_deinit();
	gsensor_deinit();
	//heat_pwm_deinit();
	

	return 0;
}
