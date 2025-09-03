This project is still in alpha so master is not stable.  
Only the multicast udp server is ready and production usable of now.  

Please note this Readme file might change quite freqently and the design of the project becomes more clear and project evolves.
                                           

Kernel-UDP-TCP: High-Speed Packet Processing with Boost::Asio and AF_XDP    
  
Overview  
  
The Kernel-UDP-TCP project aims to integrate the power of Boost::Asio for TCP/UDP flow management
with AF_XDP (Accelerated Framework for eXpress Data Path) to enable high-speed  
packet processing in a Linux kernel environment. This project is designed for developers and engineers who need efficient,
low-latency networking solutions for high-performance applications, such as real-time data streaming, network monitoring,
or low-latency trading systems. 
        


End Goal  
  
The end goal of this project is that if we have a tcp/udp server using asio (io_uring only) and we need  
to enable kernel bypass we should be able to do that via some macro like ASIO_KERNEL_BYPASS.
Asio internally uses service locator pattern to detect io_uring is present and uses it to transmit data.
Asio should be able to directly interact with the hardware and  NIC's memory-mapped queues  
using the same service pattern . So instead of going via libraries like dpdk  asio can directly access  
the kernel ring buffers and fetch/transmit data.
Even internally dpdk is just a wrapper around drivers and the ring buffers and af_xdp module but since  
it tries to support multiple os/ hardware/ nic cards and cpus the dpdk source code becomes quite complex to
debug /understand and customize if needed.


Important Notes:  
https://docs.kernel.org/networking/af_xdp.html

Kernel Debugging:

https://docs.kernel.org/dev-tools/index.html


   

Kernel Requirements:  
 linux kernel 6 or above(uring interface needed)  
i have 6.8.0-79-generic  





Current Features and flow :  
As of now master has a production grade udp multicast async server using asio (iouring interface.) (without kernel bypass)

In development Features:  
As of now when we get the packet it is io_uring interface to the kernel which actually pushes  
data.
https://github.com/axboe/liburing This is used internally by asio to talk to the kernel. 

https://docs.kernel.org/networking/af_xdp.html af spec very good for ring buffers.We  need same api and producer side.  



Libraries used by dpdk for packet processing:   
    librte_eal 
    VFIO  modprobe vfio-pci  
    UIO modprobe uio_pci_generic  


1. Create a build directory and run CMake:    
   git clone https://github.com/thisisamardeep/kernel-udp-tcp.git  
   cd kernel-udp-tcp   
   mkdir build   
   cd build    
   cmake ..    
   make      
           
2. Steps to run udp multi cast server:   
    ./udp_server_without_kernel_bypass    
                                           
3. Steps to run udp client :  
    open a new terminal and then run below command.Make sure you are in the build directory.
    ./udp_client_without_kernel_bypass  
                                                  
4. Type some message in the server terminal and it will appear in client terminal/s.






