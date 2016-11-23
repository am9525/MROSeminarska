//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package seminarska1;

simple Router
{
    parameters:
        string ipAddress = default("");
        int queueCapacity = default(20);
        int resources = default(5);
        @display("i=block/routing");
    gates:
        input sourceConn;
        output sinkConn;
        inout routerConn[];
}
simple Sink
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;
}
simple Source
{
    parameters:
        @display("i=block/source");
        double interArrivalTime = default(1);
    gates:
        output out;
}
network Omrezje
{
    @display("bgb=653,445");
    submodules:
        source: Source {
            @display("p=31,185");
        }
        sink: Sink {
            @display("p=31,131");
        }
        router: Router {
            @display("p=137,131");
        }
        router1: Router {
            @display("p=312,131");
        }
        router2: Router {
            @display("p=504,131");
        }
        router3: Router {
            @display("p=137,280");
        }
        router4: Router {
            @display("p=312,280");
        }
        router5: Router {
            @display("p=504,280");
        }
        sink1: Sink {
            @display("p=281,39");
        }
        source1: Source {
            @display("p=340,39");
        }
        sink2: Sink {
            @display("p=593,131");
        }
        source2: Source {
            @display("p=593,185");
        }
        sink3: Sink {
            @display("p=31,266");
        }
        source3: Source {
            @display("p=31,319");
        }
        sink4: Sink {
            @display("p=287,356");
        }
        source4: Source {
            @display("p=340,356");
        }
        sink5: Sink {
            @display("p=593,330");
        }
        source5: Source {
            @display("p=593,273");
        }
    connections:
		//router 1 connections
        router.routerConn++ <--> router1.routerConn++;
        router.routerConn++ <--> router3.routerConn++;
        //router 2 connections
        router1.routerConn++ <--> router2.routerConn++;
        router1.routerConn++ <--> router4.routerConn++;
        //router 3 connections
        router2.routerConn++ <--> router5.routerConn++;
        //router 4 conneections
        router3.routerConn++ <--> router4.routerConn++;
        //router 5 connections
        router4.routerConn++ <--> router5.routerConn++;
        source.out --> router.sourceConn;
        router.sinkConn --> sink.in;
        source1.out --> router1.sourceConn;
        router1.sinkConn --> sink1.in;
        source2.out --> router2.sourceConn;
        router2.sinkConn --> sink2.in;
        source5.out --> router5.sourceConn;
        router5.sinkConn --> sink5.in;
        source4.out --> router4.sourceConn;
        source3.out --> router3.sourceConn;
        router3.sinkConn --> sink3.in;
        router4.sinkConn --> sink4.in;
}
