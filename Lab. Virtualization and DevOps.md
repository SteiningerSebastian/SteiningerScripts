
Google's Gemini assisted with the development of this script.

# Introduction
Virtualization has revolutionized the way we think about computing resources. By creating virtual environments on physical hardware, we can efficiently allocate and manage resources, leading to improved scalability, flexibility, and cost-effectiveness. DevOps, on the other hand, is a cultural shift that emphasizes collaboration between development and operations teams to automate and streamline the software delivery process.

In this course, we will explore the fundamental concepts of virtualization, including hypervisors, virtual machines, and containers. By understanding these technologies and practices, you will be equipped to design, implement, and manage virtualized environments and streamline your software development and delivery pipelines.  

Throughout the course, we will use practical examples and hands-on exercises to reinforce our learning. You will have the opportunity to experiment with popular virtualization platforms, gaining valuable experience that will prepare you for real-world applications.
 
Until now, we've primarily focused on developing applications on our local machines. While this is essential for prototyping and testing, the ultimate goal is to deploy these applications to production environments where they can be accessed by users. In this course, we'll explore the techniques and best practices involved in bringing your applications from development to production.

[![[Welcome to Production](https://img.youtube.com/vi/NaR8WlLtPw0/0.jpg)](https://www.youtube.com/watch?v=NaR8WlLtPw0)

If you have headphones and are corrently working alone on a task, you can listen to this, if you would like to.
[![Welcome to Production](https://img.youtube.com/vi/HlTBGgQgNL4/0.jpg)](https://www.youtube.com/watch?v=HlTBGgQgNL4) []

## Download Docker
Follow the instructions from the link and install Docker Desktop. [Docker Desktop](https://docs.docker.com/desktop/)

After finishing the installation try to run the following command in the terminal to verify that docker was installed correctly.

```
docker --version
```

# DevOps
DevOps, a cultural shift that emphasizes collaboration between development and operations teams, has significantly benefited from the integration of virtualization technologies. Virtualization, by creating virtual environments on physical hardware, offers a flexible and efficient way to manage computing resources. This synergy between DevOps and virtualization has led to several advantages:

**Improved resource allocation and utilization:** Virtualization allows for the dynamic creation and management of virtual machines, enabling organizations to allocate resources more efficiently based on demand. This flexibility is crucial in DevOps environments where rapid changes and scaling are common.

**Faster application deployment and testing:** By leveraging virtualization, development teams can quickly create and provision virtual environments for testing and development purposes. This accelerates the application development lifecycle and enables more frequent deployments.

**Enhanced infrastructure management:** Virtualization simplifies infrastructure management tasks by providing a centralized platform for controlling and monitoring virtual machines. This automation and standardization align well with DevOps principles, reducing manual errors and improving efficiency.

**Improved disaster recovery and business continuity:** Virtualization can be used to create redundant virtual environments, providing a failover mechanism in case of hardware failures or other disruptions. This helps ensure business continuity and minimizes downtime.

**Increased scalability:** Virtualization allows organizations to easily scale their infrastructure up or down to meet changing demands. This scalability is essential in DevOps environments where applications may experience fluctuations in traffic or usage.

**Improved resource isolation:** Virtualization ensures that resources are isolated between different virtual machines, preventing one application from impacting the performance of others. This isolation is crucial in DevOps environments where multiple applications may be running concurrently on a single physical server.

Overall, the integration of virtualization and DevOps has revolutionized the way organizations manage their IT environments. By combining the flexibility and efficiency of virtualization with the collaborative and agile principles of DevOps, organizations can achieve faster time-to-market, improved application quality, and greater operational efficiency.

# Virtualization

## Introduction to Virtualization: Containers vs. Virtual Machines

Virtualization is a technology that allows multiple operating systems to run concurrently on a single physical machine. This provides significant benefits such as improved resource utilization, flexibility, and scalability. There are two primary approaches to virtualization: containers and virtual machines.

### Virtual Machines (VMs)

A virtual machine is a complete software emulation of a physical computer. It includes a virtual CPU, memory, storage, and network interface. VMs are typically created using a hypervisor, which acts as a layer between the physical hardware and the virtual machines.

**Key characteristics of VMs:**

- **Full isolation:** Each VM has its own isolated environment, including its own operating system and applications.
- **Heavyweight:** VMs require more system resources compared to containers due to the emulation of the entire hardware stack.
- **Slower boot time:** VMs can take longer to start up compared to containers due to the need to load the entire guest operating system.

#### Hypervisors: Bare-Metal vs. Software

**Hypervisors** are the software that manage and control virtual machines. They can be categorized into two main types: bare-metal and software.

##### Bare-Metal Hypervisors

Bare-metal hypervisors are installed directly on the physical hardware, bypassing the host operating system. This provides a more direct and efficient interaction with the underlying hardware, leading to better performance and resource utilization.

**Examples of bare-metal hypervisors:**

- **VMware ESXi**
- **Microsoft Hyper-V**
- **Red Hat KVM**

##### Hosted Hypervisors

Software hypervisors, also known as hosted hypervisors, run on top of a host operating system. This approach offers greater flexibility but can introduce additional overhead, potentially impacting performance.

**Examples of software hypervisors:**

- **Oracle VM VirtualBox**
- **Parallels Desktop**
- **QEMU**

**Key differences between bare-metal and software hypervisors:**

|Feature|Bare-Metal Hypervisor|Software Hypervisor|
|---|---|---|
|Installation|Directly on physical hardware|On top of a host operating system|
|Performance|Generally better due to direct hardware access|Can be slightly slower due to the additional layer of abstraction|
|Flexibility|Less flexible, as it requires dedicated hardware|More flexible, as it can run on various host operating systems|
|Resource utilization|More efficient due to direct hardware access|May have slightly higher overhead|

In summary, bare-metal hypervisors offer superior performance and resource utilization, while software hypervisors provide greater flexibility. The choice between the two depends on factors such as performance requirements, hardware constraints, and desired level of flexibility.

### Containers

Containers are a lighter-weight form of virtualization that shares the host operating system's kernel. They package applications and their dependencies into a single unit that can be easily moved and deployed across different environments.

**Key characteristics of containers:**

- **Shared kernel:** Containers share the host operating system's kernel, reducing overhead and improving performance.
- **Lightweight:** Containers are much smaller and require fewer resources than VMs.
- **Faster startup:** Containers can start up much more quickly than VMs due to the shared kernel and reduced overhead.
- **Portability:** Containers can be easily moved and deployed across different environments without modification.

**In summary,** virtual machines offer complete isolation and flexibility, but they can be resource-intensive and slow to start up. Containers, on the other hand, are lightweight, fast, and portable, but they may not provide the same level of isolation as VMs. The choice between VMs and containers depends on the specific requirements of your application, such as performance, isolation, and portability.

![Containers vs Virtual Machines](./files/virtualizationVsContainerization.drawio.svg)
Inspired by Doug Jones (2016)

## Containerization
**Containerization** is a technology that packages applications and their dependencies into a single, portable unit called a container. This approach offers several benefits, including improved portability, consistency, efficiency, isolation, and scalability.

**Key components of containerization:**

- **Dockerfile:** A text document that defines the instructions for building a container image.
- **Container image:** A snapshot of the application and its dependencies, created from a Dockerfile.
- **Container:** A running instance of a container image.

**Popular containerization platforms:**

- **Docker:** The most widely used containerization platform.
- **Kubernetes:** A container orchestration platform for managing and scaling containerized applications.

By understanding the basics of containerization, you can leverage this technology to improve the development, deployment, and management of your applications.

### Commands
To work with docker we need to know the basic commands and what they do.

**Basic Commands:**

- **docker run \[image]:** Starts a new container from a specified image.
- **docker ps:** Lists all running containers.
- **docker stop \[container_id]:** Stops a running container.
- **docker rm \[container_id]:** Removes a stopped container.  

**Image Management:**

- **docker image ls:** Lists all images on your system.
- **docker pull \[image]:** Pulls an image from a registry (like Docker Hub).
- **docker push \[image]:** Pushes an image to a registry.
- **docker build -t \[image_name] .:** Builds a new image from a Dockerfile in the current directory.
- **docker image prune -all:** Removes all unused images.

**Container Inspection and Management:**

- **docker exec -it \[container_id] \[command]:** Executes a command inside a running container.
- **docker inspect \[container_id]:** Provides detailed information about a container.
- **docker logs\[container_id]:** Shows logs generated by a container.

**Additional Commands:**

- **docker network ls:** Lists all networks.
- **docker network create \[network_name]:** Creates a new network.
- **docker network connect \[network_name] \[container_id]:** Connects a container to a network.

### Docker
#### Docker-File
##### Commands

A Dockerfile is a text document that contains instructions for building a Docker image. Here's a summary of the most commonly used commands:

**Base Image:**

- **FROM \[image]:** Specifies the base image to use for the new image.

**Build-time Instructions:**

- **COPY \[src] \[dest]:** Copies files from the host to the container.
- **ADD \[src] \[dest]:** Copies files from the host to the container, with automatic extraction for archives.
- **RUN \[command]:** Executes a command in the container.
- **WORKDIR \[path]:** Sets the working directory for subsequent commands.
- **ENV \[key=value]:** Sets environment variables.
- **ARG \[key=value]:** Defines arguments that can be passed when building the image.

**Multi-stage Builds:**

- **FROM \[image] as \[stage_name]:** Creates a new build stage with a specified name.
- **COPY --from=\[stage_name] \[src] [dest]:** Copies files from a previous stage to the current stage.

**Exposing Ports:**

- **EXPOSE \[port]:** Exposes a port for networking.

**Entrypoint and CMD:**

- **ENTRYPOINT \["command"]:** Sets the default command to run when the container starts.
- **CMD \["command"]:** Sets the default command to run if no other command is specified.

**Volume Mounts:**

- **VOLUME \[path]:** Creates a volume mount point.

**User and Group:**

- **USER \[user]:** Sets the user to run commands as.

##### Example 1: Minecraft Server
Let's build a game server. This is a great way to experience firsthand how Docker can be used to create and manage complex applications.

```
#Choose the base-build.
FROM debian:latest

# Install OpenJDK-8 Inspired by: https://stackoverflow.com/questions/31196567/installing-java-in-docker-image

RUN apt-get update && \
  apt-get install wget -y && \
  wget https://download.oracle.com/java/21/latest/jdk-21_linux-x64_bin.deb && \
  dpkg -i jdk-21_linux-x64_bin.deb && \
  apt-get install -y ant && \
  apt-get clean;

# Set the working directory and copy the .jar file to the container.
WORKDIR /minecraft

ADD https://piston-data.mojang.com/v1/objects/59353fb40c36d304f2035d51e7d6e6baa98dc05c/server.jar ./server.jar

# Run the .jar, this will create a run.sh.
RUN java -jar server.jar --installServer

RUN touch ./run.sh
RUN echo '#!/bin/bash \n java -jar ./server.jar' > ./run.sh
RUN chmod +x ./run.sh

# Executing the run.sh will create some necessary files.
RUN ./run.sh

# Next it is necessary to accept the eula to start the server.
RUN echo 'eula=true' > ./eula.txt

# To connect to the server a port must be made accesible.
EXPOSE 25565

# Start the server.
ENTRYPOINT [ "./run.sh" ]
```

This code snippet essentially creates a set of instructions for building a Docker image that can run a Minecraft server. Let's break it down step by step:

**1. Choosing a Base Image:**

- The first line `FROM debian:latest` tells Docker to use the latest version of the "debian" image as the starting point for our Minecraft server image. This image provides a basic Linux operating system environment for our server to run on.

**2. Installing Java:**

- The next section (`RUN apt-get update...`) installs the necessary software to run Java applications. It does this by updating the list of available software packages, installing `wget` (a tool for downloading files), downloading the latest OpenJDK-8 installer, installing the downloaded package, installing the `ant` build tool (potentially needed by Minecraft), and finally cleaning up the downloaded files.

**3. Setting Up the Container Environment:**

- `WORKDIR /minecraft` sets the working directory inside the container to `/minecraft`.
- `ADD https://piston-data.mojang.com/v1/objects/... server.jar` downloads the Minecraft server jar file from the official Mojang website and places it in the `/minecraft` directory inside the container with the name `server.jar`.

**4. Preparing the Server Startup Script:**

- The next few lines (`RUN java -jar...`) are a bit more involved. They:
    - Run the Minecraft server jar file with the `--installServer` flag, which likely creates some initial configuration files needed for the server to run.
    - Create an empty file called `run.sh`.
    - Fill the `run.sh` file with a simple script that starts the Minecraft server jar file using the `java -jar` command.
    - Make the `run.sh` file executable, allowing it to be run as a program.

**5. Initializing the Server and Exposing the Port:**

- `RUN ./run.sh` actually runs the `run.sh` script we just created, which likely initializes the server and creates some additional files needed for it to function.
- `RUN echo 'eula=true' > ./eula.txt` writes `eula=true` to the file. This is an agreement that needs to be accepted to run the Minecraft server.
- `EXPOSE 25565` tells Docker that the container will listen on port 25565, which is the default port used by Minecraft servers to communicate with clients.

**6. Starting the Server:**

- Finally, `ENTRYPOINT ["./run.sh"]` sets the default command to run when the container starts. In this case, it tells the container to run the `./run.sh` script, which in turn starts the Minecraft server.

In summary, this Dockerfile builds an image that can run a Minecraft server by installing Java, downloading the server files, creating a startup script, initializing the server, and finally starting it when the container is run.

**Understanding the `docker build` Command:**

The `docker build` command is used to create a new Docker image from a Dockerfile. In this case, we're using the following command:

```
docker build -t minecraft_1_21_1 .
```

- **`-t minecraft_1_21_1`:** This flag specifies the tag for the newly created image. The tag may contain a namespace (similar to a username) followed by a repository name (like a project name) and a tag (like a version).

**Building the Image:**

When you execute this command, Docker will:

1. **Read the Dockerfile:** It starts by reading the Dockerfile you've created.
2. **Execute Instructions:** Docker then executes the instructions defined in the Dockerfile, step by step. This includes downloading the base image, installing packages, copying files, and running commands.
3. **Create the Image:** Once all the instructions are executed successfully, Docker creates a new image and saves it to your local Docker repository.

**Verifying the Image:**

After the build completes, you can list all your Docker images using the following command:

Bash

```
docker images
```

You should see the newly created image listed with its tag `minecraft_1_21_1`.

**Note:** If you encounter any errors during the build process, check the Dockerfile for any syntax errors or issues with the instructions. You might also need to adjust the commands or file paths based on your specific environment or requirements

To create a new container and start it, you can use the `docker run` command to launch the server.
```
docker run -p 25565:25565 se.stei/minecraft_1_21_1
```

You can view the running container by using this command.
```
docker container ls
```

#### MORE EXAMPLES FOR STUDENTS TO TRY ON THEIR OWN ...

#### Docker-Compose
...

### Orchestration
...

# Bibliography
Doug Jones, March 16, 2018, Containers vs. Virtual Machines (VMs): What's the Difference? https://www.netapp.com/blog/containers-vs-vms/