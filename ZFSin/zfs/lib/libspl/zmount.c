/*
* CDDL HEADER START
*
* The contents of this file are subject to the terms of the
* Common Development and Distribution License, Version 1.0 only
* (the "License").  You may not use this file except in compliance
* with the License.
*
* You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
* or http://www.opensolaris.org/os/licensing.
* See the License for the specific language governing permissions
* and limitations under the License.
*
* When distributing Covered Code, include this CDDL HEADER in each
* file and include the License file at usr/src/OPENSOLARIS.LICENSE.
* If applicable, add the following below this CDDL HEADER, with the
* fields enclosed by brackets "[]" replaced with your own identifying
* information: Portions Copyright [yyyy] [name of copyright owner]
*
* CDDL HEADER END
*/
/*
* Copyright (c) 2017 Jorgen Lundman <lundman@lundman.net.  All rights reserved.
*/

/*
 * This file implements Solaris compatible zmount() function.
 */


#include <sys/mount.h>
#include <sys/zfs_mount.h>
#include <libzfs_impl.h>
#include <sys/zfs_ioctl.h>
#include <sys/w32_types.h>


int
zmount(zfs_handle_t *zhp, const char *dir, int mflag, char *fstype,
	char *dataptr, int datalen, char *optptr, int optlen)
{
	int ret = 0;

	// mount 'spec' "tank/joe" on path 'dir' "/home/joe".
	fprintf(stderr, "zmount running\r\n"); fflush(stderr);
	zfs_cmd_t zc = { "\0" };

	(void)strlcpy(zc.zc_name, zhp->zfs_name, sizeof(zc.zc_name));
	(void)strlcpy(zc.zc_value, dir, sizeof(zc.zc_value));

	ret = zfs_ioctl(zhp->zfs_hdl, ZFS_IOC_MOUNT, &zc);

	fprintf(stderr, "zmount(%s,%s) returns %d\n",
		zhp->zfs_name, dir, ret);

	fprintf(stderr, "'%s' mounted on %s\r\n", zc.zc_name, zc.zc_value);

#if 1
	char out[MAXPATHLEN];
	DWORD outlen;

	if (QueryDosDevice(
		"D:",
		out, MAXPATHLEN) > 0)
		fprintf(stderr, "'%s' mounted on %s\r\n", zc.zc_name, zc.zc_value);
	else
		fprintf(stderr, "QueryDos getlast 0x%x\n", GetLastError());

	outlen = 0;
	// these give error 0x57 (invalid parameter)
	//char *name = "\\\\?\\ZFS{0b1bb601-af0b-32e8-a1d2-54c167af6277}\\";
	//char *name = "\\\\?\\Device\\ZFS{0b1bb601-af0b-32e8-a1d2-54c167af6277}\\";
	//char *name = "\\\\?\\Device\\Volume{0b1bb601-af0b-32e8-a1d2-54c167af6277}\\";
	//char *name = "\\\\?\\DosDevices\\Global\\Volume{0b1bb601-af0b-32e8-a1d2-54c167af6277}\\";
	// this gives error 0x1 ERROR_INVALID_FUNCTION
	//char *name = "\\\\?\\Volume{0b1bb601-af0b-32e8-a1d2-54c167af6277}\\";

	char *name = "\\\\?\\Volume{5d7612f2-339b-11e7-baa7-ab3bc3128e46}\\";
	fprintf(stderr, "Looking up '%s'\r\n", name);
	ret = GetVolumePathNamesForVolumeName(name, out, MAXPATHLEN, &outlen);

	fprintf(stderr, "GetVolumePathNamesForVolumeName ret %d outlen %d GetLastError 0x%x\n", ret, outlen, GetLastError());
	if (outlen > 0 && ret > 0) {
		char *NameIdx;
		fprintf(stderr, "%s: ", zc.zc_name);
		for (NameIdx = out;
			NameIdx[0] != '\0';
			NameIdx += strlen(NameIdx) + 1)
		{
			fprintf(stderr, "  %s", NameIdx);
		}
		fprintf(stderr, "\r\n");
	}
	fprintf(stderr, "Trying mountmgr\r\n");
#define MOUNTMGR_DOS_DEVICE_NAME L"\\\\.\\MountPointManager"
	typedef struct _MOUNTMGR_MOUNT_POINT {
		ULONG  SymbolicLinkNameOffset;
		USHORT SymbolicLinkNameLength;
		ULONG  UniqueIdOffset;
		USHORT UniqueIdLength;
		ULONG  DeviceNameOffset;
		USHORT DeviceNameLength;
	} MOUNTMGR_MOUNT_POINT, *PMOUNTMGR_MOUNT_POINT;
#define MOUNTMGRCONTROLTYPE ((ULONG) 'm')
#define IOCTL_MOUNTMGR_QUERY_POINTS CTL_CODE(MOUNTMGRCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

	HANDLE mgr = CreateFileW(MOUNTMGR_DOS_DEVICE_NAME, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, INVALID_HANDLE_VALUE);
	if (mgr == INVALID_HANDLE_VALUE) return ret;

	MOUNTMGR_MOUNT_POINT input = { 0 };
	outlen = 0;
	fprintf(stderr, "sending\r\n");
	if (DeviceIoControl(mgr, IOCTL_MOUNTMGR_QUERY_POINTS, &input, sizeof(input),
		out, MAXPATHLEN, &outlen, NULL) != 0) {
		fprintf(stderr, "mountmgr returned success %d\r\n", outlen);
	}
	fprintf(stderr, "return outlen %d: GetLastError %d\r\n", outlen, GetLastError());
	CloseHandle(mgr);
#endif

	return ret;
}



int
zunmount(zfs_handle_t *zhp, const char *dir, int mflag)
{
	int ret = 0;

	// mount 'spec' "tank/joe" on path 'dir' "/home/joe".
	fprintf(stderr, "zunmount(%s,%s) running\r\n",
		zhp->zfs_name, dir); fflush(stderr);
	zfs_cmd_t zc = { "\0" };

	(void)strlcpy(zc.zc_name, zhp->zfs_name, sizeof(zc.zc_name));
	(void)strlcpy(zc.zc_value, dir, sizeof(zc.zc_value));

	ret = zfs_ioctl(zhp->zfs_hdl, ZFS_IOC_UNMOUNT, &zc);

	fprintf(stderr, "zunmount(%s,%s) returns %d\n",
		zhp->zfs_name, dir, ret);

	return ret;
}