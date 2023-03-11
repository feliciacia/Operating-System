// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
	int	type = kernel->machine->ReadRegister(2);
	int	val;
	int	i;
	char*	res;
	char*	str = "[B10815009_Log]";
	char*	file = "NachOS.log";
	char*	error = "[B10815009_Log]error\n\0";
	OpenFile* fileopen;
	FileSystem* filesystem;

    switch (which) {
	case SyscallException:
	    switch(type) {
		case SC_Halt:
		    DEBUG(dbgAddr, "Shutdown, initiated by user program.\n");
   		    kernel->interrupt->Halt();
		    break;
		case SC_PrintInt:
			val=kernel->machine->ReadRegister(4);
			cout << "Print integer:" <<val << endl;
			return;
		case SC_Example:
			val=kernel->machine->ReadRegister(4);
			cout<<"Example value:"<<val<<endl;
			return;
		case SC_ThreadYield:
			DEBUG(dbgAddr, "ThreadYield\n");
			cout<<"ThreadYield has been called\n";
			kernel->currentThread->Yield();
			return;
		case SC_Log:
			fileopen = filesystem->Open(file);
			if(!fileopen)filesystem->Create(file);
			val=kernel->machine->ReadRegister(4);
			if(val=='j' || val== 'J'){
				fileopen->WriteAt(error, strlen(error),fileopen->Length());
			}
			else{
				res = new char[strlen(str)+3];
				for(i=0;i<strlen(str);i++)res[i]=str[i];
				res[strlen(str)]=val;
				res[strlen(str)+1]='\n';
				res[strlen(str)+2]='\0';
				fileopen->WriteAt(res, strlen(res), fileopen->Length());
			    }
			return;
				
					


			
/*		case SC_Exec:
			DEBUG(dbgAddr, "Exec\n");
			val = kernel->machine->ReadRegister(4);
			kernel->StringCopy(tmpStr, retVal, 1024);
			cout << "Exec: " << val << endl;
			val = kernel->Exec(val);
			kernel->machine->WriteRegister(2, val);
			return;

*/		case SC_Exit:
			DEBUG(dbgAddr, "Program exit\n");
			val=kernel->machine->ReadRegister(4);
			cout << "return value:" << val << endl;
			kernel->currentThread->Finish();
			break;
		default:
		    cerr << "Unexpected system call " << type << "\n";
 		    break;
	    }
	    break;
	case PageFaultException:
		/*    Page Fault Exception    */
	    break;
	default:
	    cerr << "Unexpected user mode exception" << which << "\n";
	    break;
    }
    ASSERTNOTREACHED();
}
