# MCX Research

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

## License

This project is licensed under the **MIT License**.

## References & Specifications

For more technical information, refer to the **3GPP specifications related** to SIP and MCPTT:

[TS 24.379](https://www.3gpp.org/ftp/Specs/archive/24_series/24.379/), [TS 24.380](https://www.3gpp.org/ftp/Specs/archive/24_series/24.380/), [TS 24.384](https://www.3gpp.org/ftp/Specs/archive/24_series/24.384/).


