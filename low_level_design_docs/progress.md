
This document tries to explain the low level details of the data gram server using asio without kernel bypass.
The purpose of this document is to make sure that end flow does not change after kernel bypass changes are merged.




This example shows a udp multicast sender.It sends user input messages to a  specified multicast group.
Clients who have joined this group via IGMP  receive those messages.
The messages are sent async means the call returns immeditely and waits for the next message.
The messages are actually sent by Asio Event Loop.
Asio event loop is multi threaded.When we implement kernel bypass when linux runs under kernel mode  we cannot use threads
since kernel threads are different from userspace threads.
But on the consumption side of the queue we should be able to use threads// this needs to be tested on kernel 6

In multicast it is the responsiblity of the router to replicate datagram to all clients.
No client state is maintained by the sender.

So we create a AsioService which is just a wrapper around asio::io_context.
AsioService needs to pass the shared pointer from the member function so it needs to inherit from enable_shared_from_this.


external/asio/asio/include/asio/detail/io_uring_service.hpp main service file.
AF_XDP needs to be on same pattern.
                                      


Pending /Inprogress Items (Major):  

Need to trace packet flow after nic card driver picks it up.  
Analyze and document the current packet flow from NIC to user space.  
draw the design diagram of  dpdk and netmap (https://github.com/luigirizzo/netmap) 
with pencil and paper using sourcetrail otherwise it will not stick  in myhead.    
Design and implement zero-copy buffer management between NIC and user space.  
Refactor the AsioService to support direct NIC access.  
thread safety and synchronization for user space packet processing.  
Integrate  with the  kernel bypass API.  
Benchmark performance before and after kernel bypass integration to prove that this all this adds real value to asio.    
after benchmarks show real benefits need to check with Chris if he would be willing to merge this to main Boost asio repo .  
https://think-async.com/Asio/index.html  

Chris will most likely not agree to merge to standlone boost  since this is only linux compatible.Boost always wants
cross compatiblity but we can make a fork for sure. Also ask him for any design suggestions.

once udp flow works all benchmarks should ideally improve 8-10 times as per dpdk benchmarks.  
Add fallback mechanisms for environments without kernel bypass support.  
try to get other people who know linux c.(check kernel mailing list)  well  get some help regarding best way
to design the ring buffers in the project and best way to debug netwrok drivers(they know much more than me since i dont read
kernel code daily.Need to ask for some pointers regarding how to find which file affects my code flow once that is done i can do all
debugging myself.finding which file to check in the kernel is 99% of the job).
linux doesnot use plain c but internally a wrapper around plain c which is quite tough to debug.
See in linux forums/mailing lists best way to debug linux version of c.

                                             
Tough Part:  
Need to add queues for tcp and state and counter managament.  
Need to make sure all cores are utilized.See the design in mtcp.  
https://github.com/mtcp-stack/mtcp  (use sourcetrail to track depdencies and then draw the flow diagram by hand. code is quite old so would need 
lot of design changes for sure)


                                               
Blockers  and learnings  

Need to see if i can debug kernel source with Clion and clang  
try kdevelop    
https://docs.kernel.org/dev-tools/index.html     


https://docs.kernel.org/process/debugging/kgdb.html  

linux debugging with ide has some inherent limitations.you need have habit to be able to read a c file  
and understand what it does so that  
in strace if u see that function you know what it does without setting breakpoint.avoid wasting time if the code area
is not related to packet or network driver just skim the code.this is a huge waste of time.
take one kernel file daily just see it.write short hote of what it does.dont spend more than 30 min on the file.
slow slowly u will know kernel source well.
this will take time.dont fret.dont block progress in other areas if u dont understnad kernel fully.no one does.
u dont need  to know the kernel source to implement bypass.u just need to write and manage the ring buffers well.

try  kgbd.    

   very good discussion for understanding how kernel devs go from a kernel bug to the  kernel code  in kernel.  

https://lore.kernel.org/all/4b4187c1-77e5-44b7-885f-d6826723dd9a@sifive.com/