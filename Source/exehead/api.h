/*
 * apih
 * 
 * This file is a part of NSIS.
 * 
 * Copyright (C) 1999-2025 Nullsoft and Contributors
 * 
 * Licensed under the zlib/libpng license (the "License");
 * you may not use this file except in compliance with the License.
 * 
 * Licence details can be found in the file COPYING.
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.
 */

#ifndef _NSIS_EXEHEAD_API_H_
#define _NSIS_EXEHEAD_API_H_

// Starting with NSIS 2.42, you can check the version of the plugin API in exec_flags->plugin_api_version
// The format is 0xXXXXYYYY where X is the major version and Y is the minor version (MAKELONG(y,x))
// When doing version checks, always remember to use >=, ex: if (pX->exec_flags->plugin_api_version >= NSISPIAPIVER_1_0) {}

#define NSISPIAPIVER_1_0 0x00010000
#define NSISPIAPIVER_CURR NSISPIAPIVER_1_0

// NSIS Plug-In Callback Messages
enum NSPIM 
{
  NSPIM_UNLOAD,    // This is the last message a plugin gets, do final cleanup
  NSPIM_GUIUNLOAD, // Called after .onGUIEnd
};

// Prototype for callbacks registered with extra_parameters->RegisterPluginCallback()
// Return NULL for unknown messages
// Should always be __cdecl for future expansion possibilities
typedef UINT_PTR (*NSISPLUGINCALLBACK)(enum NSPIM);

// extra_parameters data structure containing other interesting stuff
// besides the stack, variables and HWND passed on to plug-ins.
typedef struct
{
  int autoclose;          // SetAutoClose
  int all_user_var;       // SetShellVarContext: User context = 0, Machine context = 1
  int exec_error;         // IfErrors/ClearErrors/SetErrors
  int abort;              // IfAbort
  int exec_reboot;        // IfRebootFlag/SetRebootFlag (NSIS_SUPPORT_REBOOT)
  int reboot_called;      // NSIS_SUPPORT_REBOOT
  int XXX_cur_insttype;   // Deprecated
  int plugin_api_version; // Plug-in ABI. See NSISPIAPIVER_CURR (Note: used to be XXX_insttype_changed)
  int silent;             // IfSilent/SetSilent (NSIS_CONFIG_SILENT_SUPPORT)
  int instdir_error;      // GetInstDirError
  int rtl;                // IfRtlLanguage: 1 if $LANGUAGE is a RTL language
  int errlvl;             // SetErrorLevel
  int alter_reg_view;     // SetRegView: Default View = 0, Alternative View = (sizeof(void*) > 4 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY)
  int status_update;      // SetDetailsPrint
} exec_flags_t;

#ifndef NSISCALL
#  define NSISCALL __stdcall
#endif
#if !defined(_WIN32) && !defined(LPTSTR)
#  define LPTSTR TCHAR*
#endif

typedef struct {
  exec_flags_t *exec_flags;
  int (NSISCALL *ExecuteCodeSegment)(int, HWND);
  void (NSISCALL *validate_filename)(LPTSTR);
  int (NSISCALL *RegisterPluginCallback)(HMODULE, NSISPLUGINCALLBACK); // returns 0 on success, 1 if already registered and < 0 on errors
} extra_parameters;

// Definitions for page showing plug-ins
// See Ui.c to understand better how they're used

// sent to the outer window to tell it to go to the next inner window
#define WM_NOTIFY_OUTER_NEXT (WM_USER+0x8)

// custom pages should send this message to let NSIS know they're ready
#define WM_NOTIFY_CUSTOM_READY (WM_USER+0xd)

// sent as wParam with WM_NOTIFY_OUTER_NEXT when user cancels - heed its warning
#define NOTIFY_BYE_BYE 'x'

#endif /* _NSIS_EXEHEAD_API_H_ */
