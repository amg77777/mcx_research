# MCX-SIP-Comm

## Overview
MCX-SIP-Comm is a **mission-critical communication system** built with **SIP (Session Initiation Protocol)** and **MCPTT (Mission-Critical Push-to-Talk)** technologies to ensure **secure, real-time voice communication** in high-reliability environments. This system is designed to provide **encrypted, concurrent, and scalable** communication, ideal for mission-critical applications that require the highest levels of reliability and security.

## Key Features
- **SIP-based signaling**: Handles call setup, session management, and signaling using the robust SIP protocol.
- **MCPTT functionality**: Implements Mission-Critical Push-to-Talk (MCPTT) for instant, reliable voice communication in high-stakes environments.
- **End-to-end encryption**: Utilizes **TLS** for signaling security and **SRTP** for encrypted media, ensuring communication confidentiality.
- **Multithreaded processing**: Uses **POSIX pthreads** to manage multiple concurrent calls, maximizing performance and responsiveness.
- **Efficient session management**: Implements advanced **data structures** such as **linked lists**, **structs**, **enums**, and **unions** for optimal session management and scalability.
- **Automated QA framework**: Includes a custom **QA automation suite** for functional and stress testing to ensure system reliability under heavy load conditions.

## Technologies & Tools Used
- **SIP Stack**: Sofia-SIP, a robust open-source SIP stack.
- **Multithreading**: POSIX **pthreads** for handling concurrent communications.
- **Security**: **TLS** for signaling encryption, **SRTP** for media encryption.
- **Programming Language**: C, leveraging low-level system capabilities for optimal performance.
- **Data Structures**: Advanced use of **structs**, **enums**, **unions**, and **linked lists** for highly efficient and scalable session management.
- **Testing Framework**: Custom-built automation tools for functional and stress testing.
- **Docker**: Integrated Docker support to build and run the system in a containerized environment, eliminating the need for `sudo` or system-wide installations and dependencies.

## Roles
- **Developer**: Responsible for implementing SIP signaling, media handling, and session management, with a focus on security and performance.
- **QA Engineer**: Develops testing strategies, creates automated test scripts, and ensures the system's reliability under varying loads.

## Installation Guide

1. **Clone the Repository**  
   Clone the repository to your local machine:
   ```sh
   git clone https://github.com/your-repo/MCX-SIP-Comm.git
   cd MCX-SIP-Comm
   
   
2. **Build with Docker**  
The project includes a Docker configuration to build the system with all dependencies pre-configured (including Sofia-SIP, OpenSSL, etc.), ensuring a smooth setup without the need for sudo or system-wide installations.

    ```sh
    make

3. **Run the application**  
Once the build completes, you can run the SIP server and client:

    ```sh
    # Start the SIP server
    ./mcx_sip_server

    # Start a client
    ./mcx_sip_client -u user1 -p 5060

## Contributors

-    Vanessa Lorenzo Toquero – Developer & QA
-    César M. Varela García – QA & Developer

## License

This project is licensed under the **MIT License**.

## References & Specifications

For more technical information, refer to the **3GPP specifications related** to SIP and MCPTT:

* [TS 24.379](https://www.3gpp.org/ftp/Specs/archive/24_series/24.379/), [TS 24.380](https://www.3gpp.org/ftp/Specs/archive/24_series/24.380/), [TS 24.384](https://www.3gpp.org/ftp/Specs/archive/24_series/24.384/).*


