//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ģ��ƽ̨  *****
//        --------------------------------------
//    	              ϵͳTRAP��������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2008-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _SYSTRAP_H
#define _SYSTRAP_H

////////////////////////////////////////////////////////////////////////
// ϵͳ����: �ں˲���
////////////////////////////////////////////////////////////////////////
#define SYSTRAP_KERNEL							0x00000001
#define SYSTRAP_PORT_CREATE						(SYSTRAP_KERNEL+0x00)
#define SYSTRAP_PORT_DESTROY					(SYSTRAP_KERNEL+0x01)
#define SYSTRAP_PORT_LOOKUP						(SYSTRAP_KERNEL+0x02)
                                        		
#define SYSTRAP_MESSAGE_SEND					(SYSTRAP_KERNEL+0x08)
#define SYSTRAP_MESSAGE_RECEIVE					(SYSTRAP_KERNEL+0x09)
#define SYSTRAP_MESSAGE_REPLY					(SYSTRAP_KERNEL+0x0A)

#define SYSTRAP_KM_INFO							(SYSTRAP_KERNEL+0x0E)

#define SYSTRAP_VM_INFO_ALL						(SYSTRAP_KERNEL+0x0F)
#define SYSTRAP_VM_ALLOC						(SYSTRAP_KERNEL+0x10)
#define SYSTRAP_VM_REALLOC						(SYSTRAP_KERNEL+0x11)
#define SYSTRAP_VM_FREE							(SYSTRAP_KERNEL+0x12)
#define SYSTRAP_VM_ALLOC_STACK					(SYSTRAP_KERNEL+0x13)
#define SYSTRAP_VM_FREE_STACK					(SYSTRAP_KERNEL+0x14)
#define SYSTRAP_VM_INFO							(SYSTRAP_KERNEL+0x15)

#define SYSTRAP_TASK_MULTIINST					(SYSTRAP_KERNEL+0x16)
#define SYSTRAP_TASK_INFO						(SYSTRAP_KERNEL+0x17)                                        		
#define SYSTRAP_TASK_CREATE						(SYSTRAP_KERNEL+0x18)
#define SYSTRAP_TASK_TERMINATE					(SYSTRAP_KERNEL+0x19)
#define SYSTRAP_TASK_SELF						(SYSTRAP_KERNEL+0x1A)
#define SYSTRAP_TASK_SUSPEND					(SYSTRAP_KERNEL+0x1B)
#define SYSTRAP_TASK_RESUME						(SYSTRAP_KERNEL+0x1C)
#define SYSTRAP_TASK_NAME						(SYSTRAP_KERNEL+0x1D)
#define SYSTRAP_TASK_MAINWND					(SYSTRAP_KERNEL+0x1E)
#define SYSTRAP_TASK_CLASS						(SYSTRAP_KERNEL+0x1F)
                                        		
#define SYSTRAP_THREAD_CREATE					(SYSTRAP_KERNEL+0x20)
#define SYSTRAP_THREAD_TERMINATE				(SYSTRAP_KERNEL+0x21)
#define SYSTRAP_THREAD_LOAD						(SYSTRAP_KERNEL+0x22)
#define SYSTRAP_THREAD_SELF						(SYSTRAP_KERNEL+0x23)
#define SYSTRAP_THREAD_YIELD					(SYSTRAP_KERNEL+0x24)
#define SYSTRAP_THREAD_SUSPEND					(SYSTRAP_KERNEL+0x25)
#define SYSTRAP_THREAD_RESUME					(SYSTRAP_KERNEL+0x26)
#define SYSTRAP_THREAD_SCHEDPARAM				(SYSTRAP_KERNEL+0x27)

#define SYSTRAP_TASK_NAME_EX					(SYSTRAP_KERNEL+0x2F)

#define SYSTRAP_TIMER_SLEEP						(SYSTRAP_KERNEL+0x30)
#define SYSTRAP_TIMER_ALARM						(SYSTRAP_KERNEL+0x31)
#define SYSTRAP_TIMER_PERIODIC					(SYSTRAP_KERNEL+0x32)
#define SYSTRAP_TIMER_WAITPERIOD				(SYSTRAP_KERNEL+0x33)

                                        		
#define SYSTRAP_EXCEPTION_SETUP					(SYSTRAP_KERNEL+0x38)
#define SYSTRAP_EXCEPTION_RETURN				(SYSTRAP_KERNEL+0x39)
#define SYSTRAP_EXCEPTION_RAISE					(SYSTRAP_KERNEL+0x3A)
#define SYSTRAP_EXCEPTION_WAIT					(SYSTRAP_KERNEL+0x3B)
                                        	                       
#define SYSTRAP_DEVICE_OPEN						(SYSTRAP_KERNEL+0x40)
#define SYSTRAP_DEVICE_CLOSE					(SYSTRAP_KERNEL+0x41)
#define SYSTRAP_DEVICE_READ						(SYSTRAP_KERNEL+0x42)
#define SYSTRAP_DEVICE_WRITE					(SYSTRAP_KERNEL+0x43)
#define SYSTRAP_DEVICE_IOCTL					(SYSTRAP_KERNEL+0x44)
#define SYSTRAP_KERNEL_MEMCPY					(SYSTRAP_KERNEL+0x47)
                                        		
#define SYSTRAP_MUTEX_CREATE					(SYSTRAP_KERNEL+0x48)
#define SYSTRAP_MUTEX_DESTROY					(SYSTRAP_KERNEL+0x49)
#define SYSTRAP_MUTEX_WAIT						(SYSTRAP_KERNEL+0x4A)
#define SYSTRAP_MUTEX_TRY						(SYSTRAP_KERNEL+0x4B)
#define SYSTRAP_MUTEX_RELEASE					(SYSTRAP_KERNEL+0x4C)
                                        		
#define SYSTRAP_COND_CREATE						(SYSTRAP_KERNEL+0x50)
#define SYSTRAP_COND_DESTROY					(SYSTRAP_KERNEL+0x51)
#define SYSTRAP_COND_WAIT						(SYSTRAP_KERNEL+0x52)
#define SYSTRAP_COND_RELEASE					(SYSTRAP_KERNEL+0x53)
                                        		
#define SYSTRAP_SEMA_CREATE						(SYSTRAP_KERNEL+0x58)
#define SYSTRAP_SEMA_DESTROY					(SYSTRAP_KERNEL+0x59)
#define SYSTRAP_SEMA_WAIT						(SYSTRAP_KERNEL+0x5A)
#define SYSTRAP_SEMA_TRY						(SYSTRAP_KERNEL+0x5B)
#define SYSTRAP_SEMA_RELEASE					(SYSTRAP_KERNEL+0x5C)
#define SYSTRAP_SEMA_VALUE						(SYSTRAP_KERNEL+0x5D)
                                        		
#define SYSTRAP_KERNEL_INFO						(SYSTRAP_KERNEL+0x60)
#define SYSTRAP_KERNEL_TICKS					(SYSTRAP_KERNEL+0x61)

#define SYSTRAP_KERN_CALLBACK_CREATE			(SYSTRAP_KERNEL+0x68)
#define SYSTRAP_KERN_CALLBACK_DESTROY			(SYSTRAP_KERNEL+0x69)
#define SYSTRAP_KERN_CALLBACK_WAIT				(SYSTRAP_KERNEL+0x6A)
#define SYSTRAP_KERN_CALLBACK_FINISH			(SYSTRAP_KERNEL+0x6B)


////////////////////////////////////////////////////////////////////////
// ϵͳ���ã�ϵͳ�ⲿ��
////////////////////////////////////////////////////////////////////////
#define SYSTRAP_SYSLIB							0x00001001
#define SYSTRAP_CREATE_PROCESS					(SYSTRAP_SYSLIB+0x00)
#define SYSTRAP_CREATE_SIM_DEBUG_PROCESS		(SYSTRAP_SYSLIB+0x01)

#define SYSTRAP_SHARE_VAR_CREATE				(SYSTRAP_SYSLIB+0x05)
#define SYSTRAP_SHARE_VAR_WRITE					(SYSTRAP_SYSLIB+0x06)
#define SYSTRAP_SHARE_VAR_READ					(SYSTRAP_SYSLIB+0x07)
                                            	
#define SYSTRAP_RTC_GET_DATE					(SYSTRAP_SYSLIB+0x08)
#define SYSTRAP_RTC_GET_TIME					(SYSTRAP_SYSLIB+0x09)
#define SYSTRAP_RTC_GET_ALARM					(SYSTRAP_SYSLIB+0x0A)
#define SYSTRAP_RTC_SET_DATE					(SYSTRAP_SYSLIB+0x0B)
#define SYSTRAP_RTC_SET_TIME					(SYSTRAP_SYSLIB+0x0C)
#define SYSTRAP_RTC_SET_ALARM					(SYSTRAP_SYSLIB+0x0D)
#define SYSTRAP_RTC_ALARM_ENA					(SYSTRAP_SYSLIB+0x0E)
#define SYSTRAP_RTC_ALARM_DIS					(SYSTRAP_SYSLIB+0x0F)
                                            	
#define SYSTRAP_UART_PRINT						(SYSTRAP_SYSLIB+0x10)
#define SYSTRAP_UART_SCAN						(SYSTRAP_SYSLIB+0x11)

#define SYSTRAP_ALARM_CREATE					(SYSTRAP_SYSLIB+0x14)
#define SYSTRAP_ALARM_DESTROY					(SYSTRAP_SYSLIB+0x15)
#define SYSTRAP_ALARM_READ						(SYSTRAP_SYSLIB+0x16)
#define SYSTRAP_ALARM_WRITE						(SYSTRAP_SYSLIB+0x17)

#define SYSTRAP_DEVCHECK_VOLTAGE_VALVE			(SYSTRAP_SYSLIB+0x18)
#define SYSTRAP_DEVCHECK_TRUSTEE				(SYSTRAP_SYSLIB+0x19)
#define SYSTRAP_DEVCHECK_SET_MSGPOST			(SYSTRAP_SYSLIB+0x1A)
#define SYSTRAP_DEVCHECK_AUTOOFF_SET			(SYSTRAP_SYSLIB+0x1B)
#define SYSTRAP_DEVCHECK_AUTOOFF_GET			(SYSTRAP_SYSLIB+0x1C)
#define SYSTRAP_DEVCHECK_BACKLIGHTOFF_SET		(SYSTRAP_SYSLIB+0x1D)
#define SYSTRAP_DEVCHECK_BACKLIGHTOFF_GET		(SYSTRAP_SYSLIB+0x1E)

#define SYSTRAP_SLEEP_ENTER_REQUEST				(SYSTRAP_SYSLIB+0x20)
#define SYSTRAP_SLEEP_ENTER_CONFIRM				(SYSTRAP_SYSLIB+0x21)
#define SYSTRAP_SLEEP_EXIT_REQUEST				(SYSTRAP_SYSLIB+0x22)
#define SYSTRAP_SLEEP_EXIT_CONFIRM				(SYSTRAP_SYSLIB+0x23)
#define SYSTRAP_SLEEP_LOGO_OFF_QUIT				(SYSTRAP_SYSLIB+0x24)
#define SYSTRAP_SLEEP_LOGO_ON_QUIT				(SYSTRAP_SYSLIB+0x25)

#define SYSTRAP_BURN_LOAD					(SYSTRAP_SYSLIB+0x28)
#define SYSTRAP_BURN_UNLOAD					(SYSTRAP_SYSLIB+0x29)
#define SYSTRAP_BURN_RUN					(SYSTRAP_SYSLIB+0x2a)
#define SYSTRAP_BURN_COMPLETE					(SYSTRAP_SYSLIB+0x2b)
#define SYSTRAP_BURN_GET_STAGE					(SYSTRAP_SYSLIB+0x2c)
#define SYSTRAP_BURN_GET_MESSAGE				(SYSTRAP_SYSLIB+0x2d)

#define SYSTRAP_APMAN_GET_CREATE				(SYSTRAP_SYSLIB+0x30)
#define SYSTRAP_APMAN_GET_DESTROY				(SYSTRAP_SYSLIB+0x31)

#define SYSTRAP_PROBE_PIN_OPEN					(SYSTRAP_SYSLIB+0x38)
#define SYSTRAP_PROBE_PIN_CLOSE					(SYSTRAP_SYSLIB+0x39)
#define SYSTRAP_PROBE_PIN_OUT					(SYSTRAP_SYSLIB+0x3a)
#define SYSTRAP_PROBE_PIN_GET					(SYSTRAP_SYSLIB+0x3b)
#define SYSTRAP_PROBE_PIN_FLP					(SYSTRAP_SYSLIB+0x3c)

#define SYSTRAP_DEVCHECK_FORCEOFF_SET				(SYSTRAP_SYSLIB+0x40)
#define SYSTRAP_DEVCHECK_FORCEOFF_GET				(SYSTRAP_SYSLIB+0x41)

#define SYSTRAP_SIM_SET_LOGO_HANDLE				(SYSTRAP_SYSLIB+0x48)
#define SYSTRAP_SET_LOGO_HANDLE					(SYSTRAP_SYSLIB+0x49)

#define SYSTRAP_AUTO_TEST_GET_COUNTER				(SYSTRAP_SYSLIB+0x50)
#define SYSTRAP_AUTO_TEST_WRITE_MSG				(SYSTRAP_SYSLIB+0x51)
#define SYSTRAP_AUTO_TEST_READ_MSG				(SYSTRAP_SYSLIB+0x52)
#define SYSTRAP_AUTO_TEST_RECORD_END				(SYSTRAP_SYSLIB+0x53)
#define SYSTRAP_AUTO_TEST_RECORD_START				(SYSTRAP_SYSLIB+0x54)
#define SYSTRAP_AUTO_TEST_PLAY_END				(SYSTRAP_SYSLIB+0x55)
#define SYSTRAP_AUTO_TEST_PLAY_START				(SYSTRAP_SYSLIB+0x56)

////////////////////////////////////////////////////////////////////////
// ϵͳ���ã��ļ�ϵͳ����
////////////////////////////////////////////////////////////////////////
#define SYSTRAP_FILESYS							0x00002001
#define SYSTRAP_FS_FERROR  						(SYSTRAP_FILESYS+0x00)
#define SYSTRAP_FS_FORMAT  						(SYSTRAP_FILESYS+0x03)
#define SYSTRAP_FS_INFO   						(SYSTRAP_FILESYS+0x04)
#define SYSTRAP_FS_INITED  						(SYSTRAP_FILESYS+0x05)
#define SYSTRAP_FS_RFSINFO   						(SYSTRAP_FILESYS+0x06)
#define SYSTRAP_FS_FORMATED  						(SYSTRAP_FILESYS+0x07)
                                            	
#define SYSTRAP_FS_FOPEN 						(SYSTRAP_FILESYS+0x08)
#define SYSTRAP_FS_FCLOSE 						(SYSTRAP_FILESYS+0x09) 
#define SYSTRAP_FS_FREMOVE  					(SYSTRAP_FILESYS+0x0A)
#define SYSTRAP_FS_FSEEK  						(SYSTRAP_FILESYS+0x0B)
#define SYSTRAP_FS_FEOF  						(SYSTRAP_FILESYS+0x0C)
#define SYSTRAP_FS_FTELL  						(SYSTRAP_FILESYS+0x0D)
#define SYSTRAP_FS_FREAD  						(SYSTRAP_FILESYS+0x0E)
#define SYSTRAP_FS_FWRITE 						(SYSTRAP_FILESYS+0x0F)  

#define SYSTRAP_FS_FRENAME 						(SYSTRAP_FILESYS+0x10)
#define SYSTRAP_FS_FMOVE 						(SYSTRAP_FILESYS+0x11)
#define SYSTRAP_FS_FPROPERTY 					(SYSTRAP_FILESYS+0x12)
#define SYSTRAP_FS_FSIZE 						(SYSTRAP_FILESYS+0x13)
#define SYSTRAP_FS_FTRIM 						(SYSTRAP_FILESYS+0x14)
                                            	                    
#define SYSTRAP_FS_DOPEN  						(SYSTRAP_FILESYS+0x18)
#define SYSTRAP_FS_DCLOSE  						(SYSTRAP_FILESYS+0x19)
#define SYSTRAP_FS_DREAD  						(SYSTRAP_FILESYS+0x1A)
#define SYSTRAP_FS_DMAKE  						(SYSTRAP_FILESYS+0x1B)
#define SYSTRAP_FS_DREMOVE  					(SYSTRAP_FILESYS+0x1C)
#define SYSTRAP_FS_DRENAME 						(SYSTRAP_FILESYS+0x1D)
#define SYSTRAP_FS_DMOVE 						(SYSTRAP_FILESYS+0x1E)

#define SYSTRAP_FS_BIOS_CHECK					(SYSTRAP_FILESYS+0x20)
#define SYSTRAP_FS_SYS_CHECK					(SYSTRAP_FILESYS+0x21)
#define SYSTRAP_FS_BIOS_INFO					(SYSTRAP_FILESYS+0x22)
#define SYSTRAP_FS_SYS_INFO						(SYSTRAP_FILESYS+0x23)

#define SYSTRAP_FS_LOW_FORMAT					(SYSTRAP_FILESYS+0x24)
#define SYSTRAP_FS_RFS_READRAND					(SYSTRAP_FILESYS+0x25)
#define SYSTRAP_FS_NAND_ID						(SYSTRAP_FILESYS+0x26)
#define SYSTRAP_FS_LIB_CHECK					(SYSTRAP_FILESYS+0x27)
#define SYSTRAP_FS_ISO_CHECK					(SYSTRAP_FILESYS+0x28)
#define SYSTRAP_FS_ISSYSFILE					(SYSTRAP_FILESYS+0x29)
#define SYSTRAP_FS_ISBIOSFILE					(SYSTRAP_FILESYS+0x2A)


////////////////////////////////////////////////////////////////////////
// ϵͳ���ã�GUI����
////////////////////////////////////////////////////////////////////////
#define SYSTRAP_GUI								0x00003001
#define SYSTRAP_WIN_OBJ_LOCK					(SYSTRAP_GUI+0x00)
#define SYSTRAP_WIN_OBJ_UNLOCK                  (SYSTRAP_GUI+0x01)

#define SYSTRAP_IS_WIN_HANDLE                   (SYSTRAP_GUI+0x08)
#define SYSTRAP_SET_USR_POINT                   (SYSTRAP_GUI+0x09)
#define SYSTRAP_GET_USR_POINT                   (SYSTRAP_GUI+0x0A)

#define SYSTRAP_FOCUS_SET                       (SYSTRAP_GUI+0x10)
#define SYSTRAP_FOCUS_GET                       (SYSTRAP_GUI+0x11)
#define SYSTRAP_FOCUS_IS_SET                    (SYSTRAP_GUI+0x12)
#define SYSTRAP_FOCUS_DISABLE                   (SYSTRAP_GUI+0x13)
#define SYSTRAP_FOCUS_ENABLE                    (SYSTRAP_GUI+0x14)
#define SYSTRAP_FOCUS_IS_ENABLE                 (SYSTRAP_GUI+0x15)
#define SYSTRAP_FOCUS_TASK		                (SYSTRAP_GUI+0x16)

#define SYSTRAP_TOUCH_SMART_ENABLE				(SYSTRAP_GUI+0x17)
#define SYSTRAP_TOUCH_SMART_DISABLE				(SYSTRAP_GUI+0x18)
#define SYSTRAP_TOUCH_SMART_IS_ENABLE			(SYSTRAP_GUI+0x19)
#define SYSTRAP_KEY_ENABLE						(SYSTRAP_GUI+0x1A)
#define SYSTRAP_KEY_DISABLE						(SYSTRAP_GUI+0x1B)
#define SYSTRAP_KEY_IS_ENABLE					(SYSTRAP_GUI+0x1C)
#define SYSTRAP_TOUCH_ENABLE					(SYSTRAP_GUI+0x1D)
#define SYSTRAP_TOUCH_DISABLE					(SYSTRAP_GUI+0x1E)
#define SYSTRAP_TOUCH_IS_ENABLE					(SYSTRAP_GUI+0x1F)

#define SYSTRAP_PAINT_INVALIDATE_WINDOWEX       (SYSTRAP_GUI+0x20)
#define SYSTRAP_PAINT_INVALIDATE_WINDOW         (SYSTRAP_GUI+0x21)
#define SYSTRAP_PAINT_INVALIDATE_HERIT          (SYSTRAP_GUI+0x22)
#define SYSTRAP_PAINT_INVALIDATE_SCREEN         (SYSTRAP_GUI+0x23)
#define SYSTRAP_PAINT_LIST_WAIT   			    (SYSTRAP_GUI+0x24)
#define SYSTRAP_PAINT_LIST_RELEASE      		(SYSTRAP_GUI+0x25)

#define SYSTRAP_WIN_ROT_ALL_WINDOWS             (SYSTRAP_GUI+0x30)
#define SYSTRAP_WIN_ROT_WINDOW_EX               (SYSTRAP_GUI+0x31)
#define SYSTRAP_WIN_ROT_WINDOW                  (SYSTRAP_GUI+0x32)
#define SYSTRAP_WIN_ROT_GET_FLAG                (SYSTRAP_GUI+0x33)
#define SYSTRAP_WIN_ROT_GET_ANG                 (SYSTRAP_GUI+0x34)
#define SYSTRAP_WIN_ROT_GET_ANG_ABS             (SYSTRAP_GUI+0x35)

#define SYSTRAP_WIN_STATUS_SHOW		         	(SYSTRAP_GUI+0x38)
#define SYSTRAP_WIN_STATUS_IS_SHOW             	(SYSTRAP_GUI+0x39)
#define SYSTRAP_WIN_STATUS_MINIMIZE            	(SYSTRAP_GUI+0x3A)
#define SYSTRAP_WIN_STATUS_IS_MINIMIZE         	(SYSTRAP_GUI+0x3B)
#define SYSTRAP_WIN_STATUS_TRANS          		(SYSTRAP_GUI+0x3C)
#define SYSTRAP_WIN_STATUS_IS_TRANS	         	(SYSTRAP_GUI+0x3D)
#define SYSTRAP_WIN_STATUS_ENABLE          		(SYSTRAP_GUI+0x3C)
#define SYSTRAP_WIN_STATUS_IS_ENABLE         	(SYSTRAP_GUI+0x3D)
                                                                
#define SYSTRAP_WIN_STATUS_MOVE                 (SYSTRAP_GUI+0x40)
#define SYSTRAP_WIN_STATUS_MOVE_TO              (SYSTRAP_GUI+0x41)
#define SYSTRAP_WIN_STATUS_RESIZE               (SYSTRAP_GUI+0x42)
#define SYSTRAP_WIN_STATUS_GET_RECT             (SYSTRAP_GUI+0x43)
#define SYSTRAP_WIN_STATUS_GET_ABS_RECT         (SYSTRAP_GUI+0x44)
#define SYSTRAP_WIN_STATUS_IS_TOP               (SYSTRAP_GUI+0x45)
#define SYSTRAP_WIN_STATUS_IS_TOP_SIBLING       (SYSTRAP_GUI+0x46)
#define SYSTRAP_WIN_STATUS_IS_BOTTOM            (SYSTRAP_GUI+0x47)
#define SYSTRAP_WIN_STATUS_IS_BOTTOM_SIBLING    (SYSTRAP_GUI+0x48)
#define SYSTRAP_WIN_STATUS_BRING_TOP            (SYSTRAP_GUI+0x49)
#define SYSTRAP_WIN_STATUS_BRING_BOTTOM         (SYSTRAP_GUI+0x4A)
#define SYSTRAP_WIN_STATUS_BRING_UP             (SYSTRAP_GUI+0x4B)
#define SYSTRAP_WIN_STATUS_BRING_DOWN           (SYSTRAP_GUI+0x4C)
#define SYSTRAP_WIN_STATUS_GET_FLAG             (SYSTRAP_GUI+0x4D)
#define SYSTRAP_WIN_STATUS_MSGQ		            (SYSTRAP_GUI+0x4E)
#define SYSTRAP_WIN_STATUS_MODAL	            (SYSTRAP_GUI+0x4F)

#define SYSTRAP_WIN_STATUS_GRAY_BUT	            (SYSTRAP_GUI+0x58)
#define SYSTRAP_WIN_STATUS_COLOR_ALL            (SYSTRAP_GUI+0x59)
#define SYSTRAP_WIN_STATUS_GET_LCD_SIZE         (SYSTRAP_GUI+0x5a)
#define SYSTRAP_WIN_STATUS_IS_TV_MODE           (SYSTRAP_GUI+0x5b)
#define SYSTRAP_WIN_GRID_SET_SWITCH				(SYSTRAP_GUI+0x5c)
                                                                
#define SYSTRAP_WIN_TIMER_CREATE                (SYSTRAP_GUI+0x50)
#define SYSTRAP_WIN_TIMER_DELETE                (SYSTRAP_GUI+0x51)
#define SYSTRAP_WIN_TIMER_TIMER                 (SYSTRAP_GUI+0x52)

#define SYSTRAP_HERIT_GET_PARENT				(SYSTRAP_GUI+0x60)
#define SYSTRAP_HERIT_GET_FIRST_CHILD			(SYSTRAP_GUI+0x61)
#define SYSTRAP_HERIT_GET_NEXT_SIBLING			(SYSTRAP_GUI+0x62)
#define SYSTRAP_HERIT_GET_PREV_SIBLING			(SYSTRAP_GUI+0x63)

#define SYSTRAP_MOUSE_GET_POS_HWND				(SYSTRAP_GUI+0x65)
#define SYSTRAP_MOUSE_SET_PAD_AREA				(SYSTRAP_GUI+0x66)
#define SYSTRAP_MOUSE_SET_MOUSE_POSITION		(SYSTRAP_GUI+0x67)
#define SYSTRAP_MOUSE_GET_MOUSE_POSITION		(SYSTRAP_GUI+0x68)
#define SYSTRAP_MOUSE_SET_MOVE_RECT				(SYSTRAP_GUI+0x69)
#define SYSTRAP_MOUSE_SET_SHAPE					(SYSTRAP_GUI+0x6A)
#define SYSTRAP_MOUSE_INIT_SHAPE				(SYSTRAP_GUI+0x6B)
#define SYSTRAP_MOUSE_GET_MODE					(SYSTRAP_GUI+0x6C)
#define SYSTRAP_MOUSE_SET_MODE					(SYSTRAP_GUI+0x6D)
#define SYSTRAP_MOUSE_GET_DISPLAY_MODE			(SYSTRAP_GUI+0x6E)
#define SYSTRAP_MOUSE_SET_DISPLAY_MODE			(SYSTRAP_GUI+0x6F)

#define SYSTRAP_MSGQ_PUT_MSG                	(SYSTRAP_GUI+0x72)
#define SYSTRAP_MSGQ_POLL_MSG               	(SYSTRAP_GUI+0x73)
#define SYSTRAP_MSGQ_GET_MSG                	(SYSTRAP_GUI+0x74)
#define SYSTRAP_MSGQ_PUT_SEND_MSG           	(SYSTRAP_GUI+0x75)
#define SYSTRAP_MSGQ_POLL_SEND_MSG          	(SYSTRAP_GUI+0x76)
#define SYSTRAP_MSGQ_GET_SEND_MSG           	(SYSTRAP_GUI+0x77)
#define SYSTRAP_MSGQ_GET_REPLY_MSG          	(SYSTRAP_GUI+0x78)
#define SYSTRAP_MSGQ_POLL_REPLY_MSG         	(SYSTRAP_GUI+0x79)
#define SYSTRAP_MSGQ_PUT_REPLY_MSG          	(SYSTRAP_GUI+0x7A)
#define SYSTRAP_MSGQ_CHECK                  	(SYSTRAP_GUI+0x7B)
#define SYSTRAP_MSGQ_DELETE		            	(SYSTRAP_GUI+0x7C)
#define SYSTRAP_MSGQ_MODAL_GET_MSG            	(SYSTRAP_GUI+0x7D)
                                           	
#define SYSTRAP_WINOBJ_CREATE               	(SYSTRAP_GUI+0x80)
#define SYSTRAP_WINOBJ_DELETE               	(SYSTRAP_GUI+0x81)
#define SYSTRAP_WINOBJ_SET_CALLBACK           	(SYSTRAP_GUI+0x82)
#define SYSTRAP_WINOBJ_GET_CALLBACK           	(SYSTRAP_GUI+0x83)
#define SYSTRAP_WINOBJ_WAIT_DEL          		(SYSTRAP_GUI+0x84)
#define SYSTRAP_WINOBJ_WAIT_DEL_CONFIRM         (SYSTRAP_GUI+0x85)
#define SYSTRAP_WINOBJ_SET_IME			        (SYSTRAP_GUI+0x86)
#define SYSTRAP_WINOBJ_GET_IME			        (SYSTRAP_GUI+0x87)

#define SYSTRAP_KEY_SHORTCUT_ADD				(SYSTRAP_GUI+0x88)
#define SYSTRAP_KEY_SHORTCUT_REDIR          	(SYSTRAP_GUI+0x89)
#define SYSTRAP_KEY_SHORTCUT_DEL            	(SYSTRAP_GUI+0x8A)
#define SYSTRAP_KEY_SHORTCUT_ENABLE         	(SYSTRAP_GUI+0x8B)
#define SYSTRAP_KEY_SHORTCUT_DISABLE        	(SYSTRAP_GUI+0x8C)
#define SYSTRAP_KEY_IS_SHORTCUT             	(SYSTRAP_GUI+0x8D)

#define SYSTRAP_TOUCH_SETTING               	(SYSTRAP_GUI+0x90)

#define SYSTRAP_GRID_SHORTCUT_ADD				(SYSTRAP_GUI+0x94)
#define SYSTRAP_GRID_SHORTCUT_REDIR          	(SYSTRAP_GUI+0x95)
#define SYSTRAP_GRID_SHORTCUT_DEL            	(SYSTRAP_GUI+0x96)
#define SYSTRAP_GRID_IS_SHORTCUT             	(SYSTRAP_GUI+0x97)

#define SYSTRAP_QUIT_REQUEST	               	(SYSTRAP_GUI+0x98)
#define SYSTRAP_QUIT_WAIT       		      	(SYSTRAP_GUI+0x99)
#define SYSTRAP_QUIT_CANCEL		               	(SYSTRAP_GUI+0x9a)

#define SYSTRAP_KEY_BINDKEY						(SYSTRAP_GUI+0x9b)
#define SYSTRAP_KEY_UNBINDKEY					(SYSTRAP_GUI+0x9C)
#define SYSTRAP_KEY_GETBINDKEY					(SYSTRAP_GUI+0x9D)

#define SYSTRAP_TOUCH_BINDTOUCH			      	(SYSTRAP_GUI+0x9e)
#define SYSTRAP_TOUCH_UNBINDTOUCH			 	(SYSTRAP_GUI+0x9f)
#define SYSTRAP_TOUCH_GETBINDTOUCH             	(SYSTRAP_GUI+0xa0)


////////////////////////////////////////////////////////////////////////
// ϵͳ���ã�GDI����
////////////////////////////////////////////////////////////////////////
#define SYSTRAP_GDI								0x00004001
#define SYSTRAP_GDI_BEGIN_PAINT					(SYSTRAP_GDI+0x00)
#define SYSTRAP_GDI_END_PAINT           		(SYSTRAP_GDI+0x01)
#define SYSTRAP_GDI_GET_CLIP_RECT          		(SYSTRAP_GDI+0x02)
#define SYSTRAP_GDI_SET_USER_DC          		(SYSTRAP_GDI+0x03)
#define SYSTRAP_GDI_GET_USER_DC          		(SYSTRAP_GDI+0x04)
#define SYSTRAP_GDI_SET_CLIP_RECT          		(SYSTRAP_GDI+0x05)
                                        		
#define SYSTRAP_WIN_GR_GET_PIXEL        		(SYSTRAP_GDI+0x10)
#define SYSTRAP_WIN_GR_DRAW_PIXEL       		(SYSTRAP_GDI+0x11)
#define SYSTRAP_WIN_GR_DRAW_HLINE       		(SYSTRAP_GDI+0x12)
#define SYSTRAP_WIN_GR_DRAW_VLINE       		(SYSTRAP_GDI+0x13)
#define SYSTRAP_WIN_GR_DRAW_BLINE       		(SYSTRAP_GDI+0x14)
#define SYSTRAP_WIN_GR_DRAW_RECT        		(SYSTRAP_GDI+0x15)
#define SYSTRAP_WIN_GR_FILL_RECT        		(SYSTRAP_GDI+0x16)
#define SYSTRAP_WIN_GR_DRAW_CHAR        		(SYSTRAP_GDI+0x17)                                       		                
#define SYSTRAP_WIN_GR_ICON_OFF         		(SYSTRAP_GDI+0x18)
#define SYSTRAP_WIN_GR_ICON_ON          		(SYSTRAP_GDI+0x19) 
                                      		
#define SYSTRAP_WIN_GR_COLOR2_INDEX     		(SYSTRAP_GDI+0x20)
#define SYSTRAP_WIN_GR_INDEX2_COLOR     		(SYSTRAP_GDI+0x21) 
                                       		
#define SYSTRAP_WIN_GR_SET_DRAW_MODE    		(SYSTRAP_GDI+0x28)
#define SYSTRAP_WIN_GR_GET_DRAW_MODE    		(SYSTRAP_GDI+0x29)
#define SYSTRAP_WIN_GR_SET_BG_COLOR     		(SYSTRAP_GDI+0x2A)
#define SYSTRAP_WIN_GR_GET_BG_COLOR     		(SYSTRAP_GDI+0x2B)
#define SYSTRAP_WIN_GR_SET_FG_COLOR     		(SYSTRAP_GDI+0x2C)
#define SYSTRAP_WIN_GR_GET_FG_COLOR     		(SYSTRAP_GDI+0x2D)
                                                               
////////////////////////////////////////////////////////////////////////
// ϵͳ���ã����岿��
////////////////////////////////////////////////////////////////////////
#define SYSTRAP_FONT							0x00005001
#define SYSTRAP_FONT_LOAD_ROM					(SYSTRAP_FONT+0x00)
#define SYSTRAP_FONT_LOAD_FILE                  (SYSTRAP_FONT+0x01)
#define SYSTRAP_FONT_LOAD_TTF                   (SYSTRAP_FONT+0x02)
#define SYSTRAP_FONT_UNLOAD                     (SYSTRAP_FONT+0x03)
#define SYSTRAP_FONT_INFO_GET                   (SYSTRAP_FONT+0x04)
#define SYSTRAP_FONT_TTF_PROPERTY				(SYSTRAP_FONT+0x05)
#define SYSTRAP_FONT_GET_CHAR_BITMAP            (SYSTRAP_FONT+0x07)
                                                                 
#define SYSTRAP_FONT_GET_CHAR_INFO              (SYSTRAP_FONT+0x08)
#define SYSTRAP_FONT_CHAR_OUT                   (SYSTRAP_FONT+0x09)
#define SYSTRAP_FONT_GET_WORD_INFO              (SYSTRAP_FONT+0x0A)
#define SYSTRAP_FONT_WORD_OUT                   (SYSTRAP_FONT+0x0B)
#define SYSTRAP_FONT_GET_STR_INFO               (SYSTRAP_FONT+0x0C)
#define SYSTRAP_FONT_STR_OUT                    (SYSTRAP_FONT+0x0D)
#define SYSTRAP_FONT_IS_IN_FONT                 (SYSTRAP_FONT+0x0E)
#define SYSTRAP_FONT_DISPLAY_HEIGHT             (SYSTRAP_FONT+0x0F)

#define SYSTRAP_FONT_OPEN		    			(SYSTRAP_FONT+0x10)
#define SYSTRAP_FONT_CLOSE                      (SYSTRAP_FONT+0x11)
#define SYSTRAP_FONT_GET_NAME                   (SYSTRAP_FONT+0x12)
#define SYSTRAP_FONT_CHECK						(SYSTRAP_FONT+0x13)

////////////////////////////////////////////////////////////////////////
// ϵͳ���ã�DIRECT����
////////////////////////////////////////////////////////////////////////
#define SYSTRAP_DIRECT							0x00006001
#define SYSTRAP_MEDIA_SRV_CREATE		    	(SYSTRAP_DIRECT+0x00)
#define SYSTRAP_MEDIA_SRV_DESTROY               (SYSTRAP_DIRECT+0x01)
#define SYSTRAP_MEDIA_SRV_CTRL                  (SYSTRAP_DIRECT+0x02)
#define SYSTRAP_MEDIA_SRV_GET_NAME				(SYSTRAP_DIRECT+0x03)

#define SYSTRAP_MEDIA_SRV_MASTER          	    (SYSTRAP_DIRECT+0x08)
#define SYSTRAP_MEDIA_SRV_INFO          	    (SYSTRAP_DIRECT+0x09)

#define SYSTRAP_SURFACE_CREATE					(SYSTRAP_DIRECT+0x10)
#define SYSTRAP_SURFACE_DESTROY					(SYSTRAP_DIRECT+0x11)
#define SYSTRAP_SURFACE_INFO					(SYSTRAP_DIRECT+0x12)
#define SYSTRAP_SURFACE_UPDATE					(SYSTRAP_DIRECT+0x13)

#define SYSTRAP_WAVEOUT_OPEN					(SYSTRAP_DIRECT+0x18)
#define SYSTRAP_WAVEOUT_CLOSE					(SYSTRAP_DIRECT+0x19)
#define SYSTRAP_WAVEOUT_SET_VOLUME				(SYSTRAP_DIRECT+0x1A)
#define SYSTRAP_WAVEOUT_WRITE					(SYSTRAP_DIRECT+0x1B)

#define SYSTRAP_OSD_OPEN						(SYSTRAP_DIRECT+0x20)
#define SYSTRAP_OSD_CLOSE						(SYSTRAP_DIRECT+0x21)
#define SYSTRAP_OSD_SET							(SYSTRAP_DIRECT+0x22)
#define SYSTRAP_OSD_WRITE						(SYSTRAP_DIRECT+0x23)

#endif // _SYSTRAP_H
