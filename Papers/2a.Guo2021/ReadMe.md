# Socker: Network-application Co-programming with Socket Tracing

* Abstract

The emerging applications and networks are putting forward more
and more demands for bi-directional awareness between applications and networks, and how to integrate applications and networks
has been studied a lot in recent years. In this paper, we observe
that socket is the waist between applications and networks, thus
we explore the possibility of integrating applications and networks
at the socket layer for the first time ever. We implement a first-ever
socket level network-application integration framework Socker
based on eBPF. By associating sockets with network control functions, programmers can realize flexible routing control based on the
application logic, as well as dynamic application logic adjustment
based on the network states. In our preliminary evaluation setting,
the result shows that the application built on Socker achieves 28.5%
request time reduction compared with the traditional socket based
implementation on average.  


**CHAT GPT explaination:**This section of the paper describes the motivation behind the development of the Socker system and the main contributions of the paper.

The section starts by stating that there is a growing demand for "bi-directional awareness" between applications and networks, meaning that applications and networks need to be able to communicate with each other and adapt to changes in the other side. This has been an active area of research in recent years, but the authors of this paper observe that the socket layer is the "waist" between applications and networks, and that it has not been explored as a way to integrate applications and networks.

The authors then introduce the Socker system, which is a framework for integrating applications and networks at the socket layer. The system is based on eBPF (extended Berkeley Packet Filter), which is a Linux kernel technology that allows for efficient tracing and manipulation of network packets. By using eBPF, the authors are able to associate sockets with network control functions, which allows programmers to create flexible routing control based on the application logic and dynamic application logic adjustment based on the network states.

The authors then present the results of an evaluation of the Socker system. They show that the application built on Socker achieves a 28.5% reduction in request time compared to a traditional socket-based implementation on average. This highlights the benefits of the system and shows that it can be a useful tool for developers working with network applications.

Overall, this section provides an overview of the problem that the Socker system aims to solve, the design of the system, and the evaluation results that demonstrate the benefits of the system.


* Introduction
  
Today’s distributed applications become more and more customized.
To be more competitive in the application market, they require high
performance with various performance metrics. Similar to applications running on standalone machines, which optimize themselves
from using the details of underlying operating system, it is a reasonable analogy that distributed applications could also improve
performance from knowing the control logic of the underlying
network. On the other hand, current networks are also becoming
increasingly "intelligent". SDN paradigm and NFV-enabled network
Therefore, the coupling of applications and network control has
become popular over the past few years. On the one side, great effort has been put in making the network aware of the application’s
requirements. For example, Google’s SDN networks [4, 11] have
already implemented control functions to accumulate application
data and make TE decisions on the data. On the other side, certain
applications also have techniques to adapt themselves to the network status, which is influenced by network control. DASH-based
applications [10] adjust their bitrates based on current bandwidth.
And in cloud gaming [12], ROI detection algorithms are based on
one-side delay. What’s more, there are also studies that combine
applications and network control in a closed loop. For example
in Minerva [8], the congestion control algorithm is based on user
QoE, and rate selection on the other hand is according to network
bandwidth.
However, all the above work assume application’s core function
logic (referred to as AF) and network control function logic (referred to as NF) are separately located in different contexts. The
internal details of AF and NF are implicit to each other. To have
close cooperation, they must additionally summarize their information into sketches and exchange the sketches with each other.
This will bring a painful dilemma as small sketches lead to loss of
information, and the cooperation will be less efficient, while large
sketches could cause large overhead in making the sketches and
passing the sketches, as well as expose too much information which
may raise security issues for both parties. Besides, they all just work
for one certain type of applications or on certain type of network
control. There lacks a general framework for joint programming of
applications and network control.
We propose a different idea from all these previous work: jointly
locate AF and NF which controls corresponding AF’s flows under the
same end host context. This way, applications and network control
can directly exchange information without any information loss,
so the overall performance will be greatly improved. Also, since
they are located under the same context at end hosts, the security
concern that applications could leak important information to the
outside no longer exists. NF could be the intermediate point for
its corresponding AF to communicate network policies with the
outside network, while keeping AF’s application details inside. Finally, this proposal depicts a unified framework for both AF and
NF inside the distributed system. In the future when the network
as a service becomes popular, it helps application users to better
understand and manage the whole system

**CHAT GPT:** This section of the paper describes the current state of distributed applications and networks, and explains the motivation behind the development of the Socker system.

The section starts by noting that today's distributed applications are becoming more and more customized, and that they require high performance with various performance metrics in order to be competitive in the application market. The authors point out that, just as applications running on standalone machines can improve performance by using the details of the underlying operating system, it is reasonable to assume that distributed applications could also improve performance by knowing the control logic of the underlying network.

The section then goes on to describe how networks are also becoming increasingly "intelligent", with the rise of Software-Defined Networking (SDN) and Network Function Virtualization (NFV) leading to a greater coupling of applications and network control. The authors note that there have been a number of efforts to make the network aware of the application's requirements, such as Google's SDN networks, which use control functions to accumulate application data and make traffic engineering decisions. They also note that some applications have techniques to adapt to the network status, such as DASH-based applications which adjust their bitrates based on current bandwidth.

The authors then point out that all of this previous work assumes that the application's core function logic (AF) and network control function logic (NF) are located in separate contexts and that the internal details of AF and NF are implicit to each other. This leads to a number of problems, such as information loss, inefficient cooperation, and security issues. They propose a different idea, which is to jointly locate AF and NF under the same end-host context. This way, applications and network control can directly exchange information without any information loss, and the overall performance will be greatly improved. Additionally, since they are located under the same context at end-hosts, security concerns that applications could leak important information to the outside no longer exist.

The authors conclude that the Socker system that they propose provides a general framework for the joint programming of applications and network control. They suggest that it will help users better understand and manage the whole system when network as a service becomes popular.
 