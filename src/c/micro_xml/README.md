# Connext Micro C Implementation using XML App Creation (MAG/appgen)

## Environment variables

The RTIMEARCH variable specifies the target architecture to build for.

    set RTIMEARCH x64Darwin17clang9.0

The RTIMEHOME environment variable is used to specify where the Connext Micro 
is installed. The libs for $RTIMEARCH are expected to be found here.
    
    set RTIMEHOME /path/to/connext/micro/home/with/libs

For example:

    set RTIMEHOME ~/rti_workspace/6.0.0/rti_connext_dds_micro-3.0.0


### Generate build system

#### Release

    mkdir -p build/Release/$RTIMEARCH

macOS (Darwin):
    
    cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" -B./build/Release/$RTIMEARCH -H. -DRTIME_TARGET_NAME=$RTIMEARCH -DPLATFORM_LIBS="dl;m;pthread" -DRTIMEHOME=$$RTIMEHOME 

#### Debug

    mkdir -p build/Release/$RTIMEARCH

macOS (Darwin):
    
    cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" -B./build/Debug/$RTIMEARCH -H. -DRTIME_TARGET_NAME=$RTIMEARCH -DPLATFORM_LIBS="dl;m;pthread" -DRTIMEHOME=$$RTIMEHOME 


### Build using the generated build system

Release:
    
    cmake --build build/Release/$RTIMEARCH

Debug:
    
    cmake --build build/Debug/$RTIMEARCH
    

## [Re]Generating type handling and application code

The generated code is located in a sub-directory: 

    gen/

The type handling code should regenerated every-time the IDL files change.
The application generation code should regenerated every-time the XML files 
change.

The `CMakeLists.txt` script includes rules so that the generated build system 
automatically regenerates the code when the corresponding source files change.



# Running with the Connext Professional Prototyper (Scripting in Lua)

- Define the list of DDS-XML files to be loaded via the `NDDS_QOS_PROFILES` 
  environment variable  
  
        export NDDS_QOS_PROFILES="res/qos/MyService_qos.xml;if/MyService.xml"
        
- Run Subscriber Example:

        rtiddsprototyper -cfgName MyServiceIfLib::Sub -luaFile src/lua/Chat_subscriber.lua
 
- Run Publisher Example:

        rtiddsprototyper -cfgName MyServiceIfLib::Pub -luaFile src/lua/Chat_publisher.lua
        

---

   Copyright 2019 Rajive Joshi, Real-Time Innovations Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.