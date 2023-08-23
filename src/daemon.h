/****************************************************************************
*    Copyright © 2014-2023 The Tumultuous Unicorn Of Darkness
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/*
* PROJECT CPU-X
* FILE daemon.h
*/

#ifndef _DAEMON_H_
#define _DAEMON_H_

#ifdef __linux__
# define SOCKET_NAME "/run/cpu-x.sock"
#else /* __linux__ */
# define SOCKET_NAME "/var/run/cpu-x.sock"
#endif /* __linux__ */
#define DAEMON_UP   (data.socket_fd >= 0)

#ifndef MAXSTR
# define MAXSTR 80 /* Max string */
#endif

#define SEND_DATA(pfd, pdata, size)    if(write(*pfd, pdata, size) != size) { MSG_ERRNO("%s", "write"); close(*pfd); *pfd = -1; return 1; }
#define RECEIVE_DATA(pfd, pdata, size) if(read (*pfd, pdata, size) != size) { MSG_ERRNO("%s", "read");  close(*pfd); *pfd = -1; return 1; }


typedef enum
{
	LIBCPUID_MSR_DEBUG,
	LIBCPUID_MSR_STATIC,
	LIBCPUID_MSR_DYNAMIC,
	DMIDECODE,
	ACCESS_DEV_PCI,
	ACCESS_SYS_DEBUG,
	LOAD_MODULE,
} DaemonCommand;

typedef struct
{
	int min_mult, max_mult, bclk;
} MsrStaticData;

typedef struct
{
	int voltage, temp;
} MsrDynamicData;

#define DMIDECODE_MAX_DIMMS 8
typedef struct
{
	int ret;
	uint8_t dimm_count;
	double bus_freq;
	char cpu_package[MAXSTR];
	char mb_manufacturer[MAXSTR], mb_model[MAXSTR], mb_revision[MAXSTR];
	char bios_brand[MAXSTR], bios_version[MAXSTR], bios_date[MAXSTR], bios_romsize[MAXSTR];
	char memory[DMIDECODE_MAX_DIMMS][MAXSTR];
} DmidecodeData;


#endif /* _DAEMON_H_ */
