/*******************************************************************************
ref: http://svn.netlabs.org/ports/ticket/13
ref: http://svn.netlabs.org/libc/ticket/204

coreutils: cannot change owner and group
Reported by: 	ydario 	Owned by: 	
Priority: 	major 	Milestone: 	
Component: 	coreutils 	Version: 	
Keywords: 		Cc: 	
Description

Current chown/chgrp implementation cannot change owner and group of files 
because of a bug in libc described in

http://svn.netlabs.org/libc/ticket/204
 
Since this code has not been incorporated in libc 0.6.4, it could be integrated
into coreutils until libc is fixed.

*******************************************************************************/


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
//#include "libc-alias.h"
#define INCL_FSMACROS
#define INCL_ERRORS
#include <os2emx.h>
//#include "b_fs.h"
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <emx/io.h>
#include <emx/syscalls.h>
#include <limits.h>
//#include "syscalls.h"
#include <InnoTekLIBC/libc.h>
#include <InnoTekLIBC/backend.h>
#define __LIBC_LOG_GROUP __LIBC_LOG_GRP_BACK_FS
#include <InnoTekLIBC/logstrict.h>

// b_fs.h
/** File EA owner. */
#define EA_UID              "UID"
/** File EA group. */
#define EA_GID              "GID"
/** Resolves and verfies the entire path. */
#define BACKFS_FLAGS_RESOLVE_FULL               0x01
/** Resolves and verfies the entire path, but don't resolve any symlink in the last component. */
#define BACKFS_FLAGS_RESOLVE_FULL_SYMLINK       0x02
/** The specified path is a directory. */
#define BACKFS_FLAGS_RESOLVE_DIR                0x10
/** Internal, use BACKFS_FLAGS_RESOLVE_DIR_MAYBE. */
#define BACKFS_FLAGS_RESOLVE_DIR_MAYBE_         0x80
/** The specified path maybe a directory. */
#define BACKFS_FLAGS_RESOLVE_DIR_MAYBE          (BACKFS_FLAGS_RESOLVE_DIR_MAYBE_ | BACKFS_FLAGS_RESOLVE_DIR)

/**
 * Change the owner and group over a file.
 *
 * @returns 0 on success.
 * @returns -1 and errno on failure.
 * @param   path    Path to the file to change owner/group of.
 * @param   owner   Owner id.
 * @param   group   Group id.
 */
int	 chown(const char *path, uid_t owner, gid_t group)
{
    LIBCLOG_ENTER("path=%p:{%s} owner=%d group=%d\n", (void *)path, path, owner, group);
    int rc = __libc_Back_fsFileOwnerSet(path, owner, group);
    if (!rc)
        LIBCLOG_RETURN_INT(0);
    errno = -rc;
    LIBCLOG_ERROR_RETURN_INT(-1);
}

/**
 * Change the owner and group over a symbolic link.
 *
 * @returns 0 on success.
 * @returns -1 and errno on failure.
 * @param   path    Path to the link to change owner/group of.
 * @param   owner   Owner id.
 * @param   group   Group id.
 */
int	 lchown(const char *path, uid_t owner, gid_t group)
{
    LIBCLOG_ENTER("path=%p:{%s} owner=%d group=%d\n", (void *)path, path, owner, group);
    int rc = __libc_Back_fsSymlinkOwnerSet(path, owner, group);
    if (!rc)
        LIBCLOG_RETURN_INT(0);
    errno = -rc;
    LIBCLOG_ERROR_RETURN_INT(-1);
}


/**
 * Sets the file access mode of a file.
 *
 * @returns 0 on success.
 * @returns Negative error code (errno.h) on failure.
 * @param   fh      Handle to file.
 * @param   Mode    The filemode.
 */
#if 0
int __libc_Back_fsSymlinkModeSet(const char *pszPath, mode_t Mode)
{
    LIBCLOG_ENTER("pszPath=%p:{%s} Mode=%#x\n", (void *)pszPath, pszPath, Mode);

    /*
     * Resolve the path.
     */
    char szNativePath[PATH_MAX];
    int rc = __libc_Back_fsPathResolve(pszPath, szNativePath, PATH_MAX, 0);
    if (!rc)
        rc = __libc_back_fsNativeFileModeSet(szNativePath, Mode);

    if (!rc)
        LIBCLOG_RETURN_INT(rc);
    LIBCLOG_ERROR_RETURN_INT(rc);
}
#endif

/**
 * Sets the file group/owner of a file.
 *
 * @returns 0 on success.
 * @returns Negative error code (errno.h) on failure.
 * @param   fh      Handle to file.
 * @param   Mode    The filemode.
 */
int __libc_Back_fsSymlinkOwnerSet(const char *pszPath, uid_t owner, gid_t group)
{
    LIBCLOG_ENTER("pszPath=%p:{%s} Owner=%#d Group=%#d\n", (void *)pszPath, pszPath, owner, group);

    /*
     * Resolve the path.
     */
    char szNativePath[PATH_MAX];
    int rc = __libc_Back_fsPathResolve(pszPath, szNativePath, PATH_MAX, 0);
    if (!rc)
        rc = __libc_back_fsNativeFileOwnerSet(szNativePath, owner, group);

    if (!rc)
        LIBCLOG_RETURN_INT(rc);
    LIBCLOG_ERROR_RETURN_INT(rc);
}


/**
 * Sets the file group/owner of a file.
 *
 * @returns 0 on success.
 * @returns Negative error code (errno.h) on failure.
 * @param   pszPath The path to the file to set the mode of.
 * @param   owner    The new owner, if -1 do not change it.
 * @param   group    The new group, if -1 do not change it.
 */
int __libc_Back_fsFileOwnerSet(const char *pszPath, uid_t owner, gid_t group)
{
    LIBCLOG_ENTER("pszPath=%p:{%s} Owner=%#d Owner=%#d\n", (void *)pszPath, pszPath, owner, group);

    /*
     * Resolve the path.
     */
    char szNativePath[PATH_MAX];
    int rc = __libc_Back_fsPathResolve(pszPath, szNativePath, PATH_MAX, 0);
    if (!rc)
        rc = __libc_back_fsNativeFileOwnerSet(szNativePath, owner, group);

    if (!rc)
        LIBCLOG_RETURN_INT(rc);
    LIBCLOG_ERROR_RETURN_INT(rc);
}


/**
 * Sets the file group/owner of a native file.
 *
 * @returns 0 on success.
 * @returns Negative error code (errno.h) on failure.
 * @param   pszNativePath   Path to the file to change.
 * @param   owner    The new owner, if -1 do not change it.
 * @param   group    The new group, if -1 do not change it.
 */
int __libc_back_fsNativeFileOwnerSet(const char *pszNativePath, uid_t owner, gid_t group)
{
    LIBCLOG_ENTER("pszNativePath=%p:{%s} Owner=%#d Group=%#d\n", (void *)pszNativePath, pszNativePath, owner, group);
    FS_VAR();

    /*
     * Validate input, refusing named pipes.
     */
    if (    (pszNativePath[0] == '/' || pszNativePath[0] == '\\')
        &&  (pszNativePath[1] == 'p' || pszNativePath[1] == 'P')
        &&  (pszNativePath[2] == 'i' || pszNativePath[2] == 'I')
        &&  (pszNativePath[3] == 'p' || pszNativePath[3] == 'P')
        &&  (pszNativePath[4] == 'e' || pszNativePath[4] == 'E')
        &&  (pszNativePath[5] == '/' || pszNativePath[5] == '\\'))
        LIBCLOG_ERROR_RETURN_INT(-ENOENT);

    /*
     * If potential device, then perform real check.
     * (Devices are subject to mode in POSIX.)
     */
    /** @todo copy device check from the path resolver. */

    /** @todo YD add right access check */

    /*
     * This call isn't available in Non-Unix mode - we don't handle symlinks there.
     * YD true?
     */
    //if (__predict_false(__libc_gfNoUnix != 0))
    //    LIBCLOG_ERROR_RETURN(-ENOSYS, "ret -ENOSYS - __libc_gfNoUnix=%d\n", __libc_gfNoUnix);

    /*
     * Do we have UnixEAs on this path?
     */
    //if (__predict_false(!__libc_back_fsInfoSupportUnixEAs(pszNativePath)))
    //    LIBCLOG_ERROR_RETURN(-ENOTSUP, "ret -ENOTSUP - no Unix EAs on '%s'\n", pszNativePath);

    /*
     * update owner if specified
     */
    FS_SAVE_LOAD();
    int rc;
    if (owner != -1)
    {
            /* construct FEA2 stuff. */
            #pragma pack(1)
            struct __LIBC_FSUNIXATTRIBSSETMODE
            {
                ULONG   cbList;
                ULONG   off;
                BYTE    fEA;
                BYTE    cbName;
                USHORT  cbValue;
                CHAR    szName[sizeof(EA_UID)];
                USHORT  usType;
                USHORT  cbData;
                uint32_t u32Mode;
            } EAs =
            {
                sizeof(EAs), 0, FEA_NEEDEA, sizeof(EA_UID) - 1, sizeof(uint32_t) + 4, EA_UID, EAT_BINARY, sizeof(uint32_t), owner
            };
            #pragma pack()
            EAOP2 EaOp2;
            EaOp2.fpGEA2List = NULL;
            EaOp2.fpFEA2List = (PFEA2LIST)&EAs;
            EaOp2.oError = 0;

            /* finally, try update / add the EA. */
            rc = DosSetPathInfo((PCSZ)pszNativePath, FIL_QUERYEASIZE, &EaOp2, sizeof(EaOp2), 0);
            if (__predict_false(rc != NO_ERROR))
            {
                LIBCLOG_ERROR("DosSetPathInfo('%s',,,,) -> %d, oError=%#lx\n", pszNativePath, rc, EaOp2.oError);
                if (rc == ERROR_EAS_NOT_SUPPORTED)
                    rc = 0;
                else
                    rc = -__libc_native2errno(rc);
            }

        if (__predict_false(rc != 0))
        {
            FS_RESTORE();
            LIBCLOG_ERROR_RETURN_INT(rc);
        }
    }
    
    /*
     * update group if specified
     */
    if (group != -1)
    {
            /* construct FEA2 stuff. */
            #pragma pack(1)
            struct __LIBC_FSUNIXATTRIBSSETMODE
            {
                ULONG   cbList;
                ULONG   off;
                BYTE    fEA;
                BYTE    cbName;
                USHORT  cbValue;
                CHAR    szName[sizeof(EA_GID)];
                USHORT  usType;
                USHORT  cbData;
                uint32_t u32Mode;
            } EAs =
            {
                sizeof(EAs), 0, FEA_NEEDEA, sizeof(EA_GID) - 1, sizeof(uint32_t) + 4, EA_GID, EAT_BINARY, sizeof(uint32_t), group
            };
            #pragma pack()
            EAOP2 EaOp2;
            EaOp2.fpGEA2List = NULL;
            EaOp2.fpFEA2List = (PFEA2LIST)&EAs;
            EaOp2.oError = 0;

            /* finally, try update / add the EA. */
            rc = DosSetPathInfo((PCSZ)pszNativePath, FIL_QUERYEASIZE, &EaOp2, sizeof(EaOp2), 0);
            if (__predict_false(rc != NO_ERROR))
            {
                LIBCLOG_ERROR("DosSetPathInfo('%s',,,,) -> %d, oError=%#lx\n", pszNativePath, rc, EaOp2.oError);
                if (rc == ERROR_EAS_NOT_SUPPORTED)
                    rc = 0;
                else
                    rc = -__libc_native2errno(rc);
            }

        if (__predict_false(rc != 0))
        {
            FS_RESTORE();
            LIBCLOG_ERROR_RETURN_INT(rc);
        }
    }

    FS_RESTORE();

    LIBCLOG_RETURN_INT(0);
}


/**
 * Sets the owner/group of a file by filehandle.
 *
 * @returns 0 on success.
 * @returns Negative error code (errno.h) on failure.
 * @param   fh      Handle to file.
 * @param   owner    The new owner, if -1 do not change it.
 * @param   group    The new group, if -1 do not change it.
 */
int __libc_Back_fsFileOwnerSetFH(int fh, uid_t owner, gid_t group)
{
    LIBCLOG_ENTER("fh=%d Owner=%#d Group=%#d\n", fh, owner, group);

    /*
     * Get filehandle.
     */
    PLIBCFH pFH;
    int rc = __libc_FHEx(fh, &pFH);
    if (rc)
        LIBCLOG_ERROR_RETURN_INT(rc);

    /*
     * Check the type.
     */
    switch (pFH->fFlags & __LIBC_FH_TYPEMASK)
    {
        /* fail */
        case F_SOCKET:
        case F_PIPE: /* treat as socket for now */
            LIBCLOG_ERROR_RETURN_INT(-EINVAL);
        /* ignore */
        case F_DEV:
            LIBCLOG_RETURN_INT(0);

        /* use the path access. */
        case F_DIR:
            if (__predict_false(!pFH->pszNativePath))
                LIBCLOG_ERROR_RETURN_INT(-EINVAL);
            rc = __libc_back_fsNativeFileOwnerSet(pFH->pszNativePath, owner, group);
            if (rc)
                LIBCLOG_ERROR_RETURN_INT(rc);
            LIBCLOG_RETURN_INT(rc);

        /* treat */
        default:
        case F_FILE:
            break;
    }

    /** @todo YD add right access check */

    if (!pFH->pOps)
    {
        /*
         * Standard OS/2 file handle.
         */
        FS_VAR();
        FS_SAVE_LOAD();
	/*
	 * update owner if specified
	 */
	FS_SAVE_LOAD();
	int rc;
	if (owner != -1)
	{
		/* construct FEA2 stuff. */
		#pragma pack(1)
		struct __LIBC_FSUNIXATTRIBSSETMODE
		{
		    ULONG   cbList;
		    ULONG   off;
		    BYTE    fEA;
		    BYTE    cbName;
		    USHORT  cbValue;
		    CHAR    szName[sizeof(EA_UID)];
		    USHORT  usType;
		    USHORT  cbData;
		    uint32_t u32Mode;
		} EAs =
		{
		    sizeof(EAs), 0, FEA_NEEDEA, sizeof(EA_UID) - 1, sizeof(uint32_t) + 4, EA_UID, EAT_BINARY, sizeof(uint32_t), owner
		};
		#pragma pack()
		EAOP2 EaOp2;
		EaOp2.fpGEA2List = NULL;
		EaOp2.fpFEA2List = (PFEA2LIST)&EAs;
		EaOp2.oError = 0;
    
		/* finally, try update / add the EA. */
		rc = DosSetFileInfo(fh, FIL_QUERYEASIZE, &EaOp2, sizeof(EaOp2));
		if (__predict_false(rc != NO_ERROR))
		{
		    LIBCLOG_ERROR("DosSetFileInfo('%d',,,,) -> %d, oError=%#lx\n", fh, rc, EaOp2.oError);
		    if (rc == ERROR_EAS_NOT_SUPPORTED)
			rc = 0;
		    else
			rc = -__libc_native2errno(rc);
		}
    
	    if (__predict_false(rc != 0))
	    {
		FS_RESTORE();
		LIBCLOG_ERROR_RETURN_INT(rc);
	    }
	}
	
	/*
	 * update group if specified
	 */
	if (group != -1)
	{
		/* construct FEA2 stuff. */
		#pragma pack(1)
		struct __LIBC_FSUNIXATTRIBSSETMODE
		{
		    ULONG   cbList;
		    ULONG   off;
		    BYTE    fEA;
		    BYTE    cbName;
		    USHORT  cbValue;
		    CHAR    szName[sizeof(EA_GID)];
		    USHORT  usType;
		    USHORT  cbData;
		    uint32_t u32Mode;
		} EAs =
		{
		    sizeof(EAs), 0, FEA_NEEDEA, sizeof(EA_GID) - 1, sizeof(uint32_t) + 4, EA_GID, EAT_BINARY, sizeof(uint32_t), group
		};
		#pragma pack()
		EAOP2 EaOp2;
		EaOp2.fpGEA2List = NULL;
		EaOp2.fpFEA2List = (PFEA2LIST)&EAs;
		EaOp2.oError = 0;
    
		/* finally, try update / add the EA. */
		rc = DosSetFileInfo(fh, FIL_QUERYEASIZE, &EaOp2, sizeof(EaOp2));
		if (__predict_false(rc != NO_ERROR))
		{
		    LIBCLOG_ERROR("DosSetFileInfo('%d',,,,) -> %d, oError=%#lx\n", fh, rc, EaOp2.oError);
		    if (rc == ERROR_EAS_NOT_SUPPORTED)
			rc = 0;
		    else
			rc = -__libc_native2errno(rc);
		}
    
	    if (__predict_false(rc != 0))
	    {
		FS_RESTORE();
		LIBCLOG_ERROR_RETURN_INT(rc);
	    }
	}
  
    FS_RESTORE();
    }

    LIBCLOG_RETURN_INT(0);
}

static unsigned char const errno_tab[] =
{
  EINVAL, EINVAL, ENOENT, ENOENT, EMFILE,  /* 0..4 */
  EACCES, EBADF,  EIO,    ENOMEM, EIO,     /* 5..9 */
  EINVAL, ENOEXEC,EINVAL, EINVAL, EINVAL,  /* 10..14 */
  ENOENT, EBUSY,  EXDEV,  ENOENT, EROFS,   /* 15..19 */
  EIO,    EIO,    EIO,    EIO,    EIO,     /* 20..24 */
  EIO,    EIO,    EIO,    ENOSPC, EIO,     /* 25..29 */
  EIO,    EIO,    EACCES, EACCES, EIO,     /* 30..34 */
  EIO,    EIO,    EIO,    EIO,    ENOSPC,  /* 35..39 */
  EIO,    EIO,    EIO,    EIO,    EIO,     /* 40..44 */
  EIO,    EIO,    EIO,    EIO,    EIO,     /* 45..49 */
  EIO,    EIO,    EIO,    EIO,    EBUSY,   /* 50..54 */
  EIO,    EIO,    EIO,    EIO,    EIO,     /* 55..59 */
  EIO,    ENOSPC, ENOSPC, EIO,    EIO,     /* 60..64 */
  EACCES, EIO,    EIO,    EIO,    EIO,     /* 65..69 */
  EIO,    EIO,    EIO,    EROFS,  EIO,     /* 70..74 */
  EIO,    EIO,    EIO,    EIO,    EIO,     /* 75..79 */
  EEXIST, EIO,    ENOENT, EIO,    EIO,     /* 80..84 */
  EIO,    EIO,    EINVAL, EIO,    EAGAIN,  /* 85..89 */
  EIO,    EIO,    EIO,    EIO,    EIO,     /* 90..94 */
  EINTR,  EIO,    EIO,    EIO,    EACCES,  /* 95..99 */
  ENOMEM, EINVAL, EINVAL, ENOMEM, EINVAL,  /* 100..104 */
  EINVAL, ENOMEM, EIO,    EACCES, EPIPE,   /* 105..109 */
  ENOENT, E2BIG,  ENOSPC, ENOMEM, EBADF,   /* 110..114 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 115..119 */
  EINVAL, EINVAL, EINVAL, ENOENT, EINVAL,  /* 120..124 */
  ENOENT, ENOENT, ENOENT, ECHILD, ECHILD,  /* 125..129 */
  EACCES, EINVAL, ESPIPE, EINVAL, EINVAL,  /* 130..134 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 135..139 */
  EINVAL, EINVAL, EBUSY,  EINVAL, EINVAL,  /* 140..144 */
  EINVAL, EINVAL, EINVAL, EBUSY,  EINVAL,  /* 145..149 */
  EINVAL, EINVAL, ENOMEM, EINVAL, EINVAL,  /* 150..154 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 155..159 */
  EINVAL, EINVAL, EINVAL, EINVAL, EAGAIN,  /* 160..164 */
  EINVAL, EINVAL, EACCES, EINVAL, EINVAL,  /* 165..169 */
  EBUSY,  EINVAL, EINVAL, EINVAL, EINVAL,  /* 170..174 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 175..179 */
  EINVAL, EINVAL, EINVAL, EINVAL, ECHILD,  /* 180..184 */
  EINVAL, EINVAL, ENOENT, EINVAL, EINVAL,  /* 185..189 */
  ENOEXEC,ENOEXEC,ENOEXEC,ENOEXEC,ENOEXEC, /* 190..194 */
  ENOEXEC,ENOEXEC,ENOEXEC,ENOEXEC,ENOEXEC, /* 195..199 */
  ENOEXEC,ENOEXEC,ENOEXEC,ENOENT, EINVAL,  /* 200..204 */
  EINVAL, ENAMETOOLONG, EINVAL, EINVAL, EINVAL,  /* 205..209 */
  EINVAL, EINVAL, EACCES, ENOEXEC,ENOEXEC, /* 210..214 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 215..219 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 220..224 */
  EINVAL, EINVAL, EINVAL, ECHILD, EINVAL,  /* 225..229 */
  EINVAL, EBUSY,  EAGAIN, ENOTCONN, EINVAL, /* 230..234 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 235..239 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 240..244 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 245..249 */
  EACCES, EACCES, EINVAL, ENOENT, EINVAL,  /* 250..254 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 255..259 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 260..264 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 265..269 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 270..274 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 275..279 */
  EINVAL, EINVAL, ENOTSUP, EINVAL, EEXIST,  /* 280..284 */
  EEXIST, EINVAL, EINVAL, EINVAL, EINVAL,  /* 285..289 */
  ENOMEM, EMFILE, EINVAL, EINVAL, EINVAL,  /* 290..294 */
  EINVAL, EINVAL, EINVAL, EINVAL, EINVAL,  /* 295..299 */
  EINVAL, EBUSY,  EINVAL, ESRCH,  EINVAL,  /* 300..304 */
  ESRCH,  EINVAL, EINVAL, EINVAL, ESRCH,   /* 305..309 */
  EINVAL, ENOMEM, EINVAL, EINVAL, EINVAL,  /* 310..314 */
  EINVAL, E2BIG,  ENOENT, EIO,    EIO,     /* 315..319 */
  EINVAL, EINVAL, EINVAL, EINVAL, EAGAIN,  /* 320..324 */
  EINVAL, EINVAL, EINVAL, EIO,    ENOENT,  /* 325..329 */
  EACCES, EACCES, EACCES, ENOENT, ENOMEM   /* 330..334 */
};


void _sys_set_errno (unsigned long rc)
{
  if (rc >= sizeof (errno_tab))
    errno = EINVAL;
  else
    errno = errno_tab[rc];
}

int __libc_native2errno (unsigned long rc)
{
  if (rc >= sizeof (errno_tab))
    return EINVAL;
  return errno_tab[rc];
}


