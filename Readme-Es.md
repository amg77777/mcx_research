# MCX-SIP-Comm

## Descripción
MCX-SIP-Comm es un **sistema de comunicación de misión crítica** construido con **SIP (Session Initiation Protocol) y MCPTT (Mission-Critical Push-to-Talk)** para garantizar **comunicación de voz segura y en tiempo real**. en entornos de alta fiabilidad. Este sistema está diseñado para proporcionar comunicación **cifrada, concurrente y escalable** , ideal para aplicaciones de misión crítica que requieren los más altos niveles de fiabilidad y seguridad.

## Características Clave
- **Señalización basada en SIP:** Maneja la configuración de llamadas, gestión de sesiones y señalización utilizando el robusto protocolo SIP.
- **Funcionalidad MCPTT:** Implementa Push-to-Talk de misión crítica (MCPTT) para comunicación instantánea y confiable en entornos de alto riesgo.
- **Cifrado de extremo a extremo:** Utiliza **TLS** para la seguridad de la señalización y **SRTP** para el cifrado de medios, garantizando la confidencialidad de la comunicación.
- **Procesamiento multihilo:** Utiliza **POSIX pthreads** para manejar múltiples llamadas concurrentes, maximizando el rendimiento y la capacidad de respuesta.
- **Gestión eficiente de sesiones:** Implementa estructuras de datos avanzadas como **listas enlazadas, structs, enums y unions** para una gestión de sesiones óptima y escalable.
- **Marco de automatización de QA:** Incluye un conjunto personalizado de **automatización de QA** para pruebas funcionales y de estrés para garantizar la fiabilidad del sistema bajo condiciones de carga pesada.

## Tecnologías y Herramientas Utilizadas
- **SIP Stack:** Sofia-SIP, una pila SIP de código abierto robusta.
- **Multihilo:** **pthreads** POSIX para manejar comunicaciones concurrentes.
- **Seguridad:** **TLS** para cifrado de señalización, **SRTP** para cifrado de medios.
- **Lenguaje de Programación:** **C**, aprovechando las capacidades de bajo nivel del sistema para un rendimiento óptimo.
- **Estructuras de Datos:** Uso avanzado de **structs, enums, unions y listas enlazadas** para una gestión de sesiones altamente eficiente y escalable.
- **Marco de Pruebas:** Herramientas de automatización personalizadas para pruebas funcionales y de estrés.
- **Docker:** Soporte integrado de Docker para construir y ejecutar el sistema en un entorno containerizado, eliminando la necesidad de sudo o instalaciones y dependencias a nivel de sistema.

## Roles
- **Desarrollador:** Responsable de implementar la señalización SIP, manejo de medios y gestión de sesiones, con enfoque en seguridad y rendimiento.
- **Ingeniero de QA:** Desarrolla estrategias de prueba, crea scripts de prueba automatizados y asegura la fiabilidad del sistema bajo diversas cargas.

## Guía de Instalación

1. **Clonar el Repositorio**  
   Clona el repositorio en tu máquina local:
   ```sh
   git clone https://github.com/vlorenzolana/MCX-SIP-Comm.git
   cd MCX-SIP-Comm
   
   
2. **Construir con Docker**  
El proyecto incluye una configuración de Docker para construir el sistema con todas las dependencias preconfiguradas (incluyendo Sofia-SIP, OpenSSL, etc.), asegurando una instalación fluida sin la necesidad de sudo ni instalaciones a nivel de sistema.
    ```sh
    make

3. **Ejecutar la aplicación**  
Una vez que la construcción se complete, puedes ejecutar el servidor SIP y el cliente:

    ```sh
    # Iniciar el servidor SIP
    ./mcx_sip_server

    # Iniciar un cliente
    ./mcx_sip_client -u user1 -p 5060


## Licencia
Este proyecto está licenciado bajo la Licencia MIT.

## Contribuidores
- Vanessa Lorenzo Toquero – Desarrolladora & QA
- César  M. Varela García – QA & Desarrollador

---
**Para más información técnica, consulta las especificaciones de 3GPP relacionadas con SIP y MCPTT:** [TS 24.379](https://www.3gpp.org/ftp/Specs/archive/24_series/24.379/), [TS 24.380](https://www.3gpp.org/ftp/Specs/archive/24_series/24.380/), [TS 24.384](https://www.3gpp.org/ftp/Specs/archive/24_series/24.384/).


