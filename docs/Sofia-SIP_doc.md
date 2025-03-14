# Integración de Sofia-SIP y oRTP

Este proyecto utiliza las bibliotecas **Sofia-SIP** y **oRTP** para establecer y gestionar llamadas SIP y la transmisión de audio en tiempo real mediante RTP. A continuación se describen las funciones principales de cada biblioteca y cómo se integran en la aplicación.

## Funciones de Sofia-SIP

### 1. Inicialización y configuración
- **sofia_initialize()**  
  Inicializa la biblioteca Sofia-SIP. Es necesario llamarla antes de usar cualquier otra función de Sofia-SIP.

- **sofia_shutdown()**  
  Cierra y limpia todos los recursos utilizados por la biblioteca. Debe llamarse al final de la aplicación para liberar los recursos.

### 2. Gestión de sesiones y llamadas
- **sofia_create_session()**  
  Crea una nueva sesión SIP. Se utiliza para gestionar el estado de una llamada.  
  **Parámetros:** Información sobre la llamada (tipo de protocolo, dirección del servidor, etc.).

- **sofia_terminate_session()**  
  Termina una sesión SIP activa. Se invoca al finalizar una llamada.

- **sofia_make_call()**  
  Realiza una llamada SIP a un destino específico, iniciando la comunicación.

- **sofia_answer_call()**  
  Responde a una llamada entrante, aceptándola para establecer la comunicación.

- **sofia_hangup_call()**  
  Cuelga una llamada activa, finalizando la comunicación en curso.

### 3. Envío y recepción de mensajes SIP
- **sofia_send_request()**  
  Envía un mensaje SIP personalizado. Se utiliza para enviar peticiones SIP como INVITE, BYE, ACK, etc.

- **sofia_receive_response()**  
  Recibe respuestas SIP para gestionar las respuestas a las peticiones, como 200 OK o 404 Not Found.

### 4. Manejo de codecs y SDP
- **sofia_set_codec()**  
  Establece el codec de audio a utilizar durante la llamada SIP. Se configura antes o durante la negociación de la sesión.

- **sofia_create_sdp()**  
  Crea un objeto SDP (Session Description Protocol) que describe la configuración de la sesión, incluidos los codecs y la información de transporte.

- **sofia_set_sdp()**  
  Asocia un objeto SDP con una sesión SIP, enviándolo para negociar las capacidades de la sesión.

### 5. Autenticación y seguridad
- **sofia_enable_tls()**  
  Habilita el soporte para TLS en las comunicaciones SIP, asegurando la conexión.

- **sofia_set_credentials()**  
  Establece las credenciales (usuario y contraseña) para la autenticación en el servidor SIP.

### 6. Eventos y Callbacks
- **sofia_set_event_handler()**  
  Establece un controlador de eventos para escuchar eventos de la sesión SIP, como llamadas entrantes y terminaciones.

## Funciones de oRTP

oRTP gestiona la transmisión de audio en tiempo real mediante el protocolo RTP. A continuación se listan sus funciones clave:

### 1. Inicialización y configuración
- **ortp_init()**  
  Inicializa la biblioteca oRTP. Debe llamarse antes de usar otras funciones de oRTP.

- **ortp_shutdown()**  
  Finaliza la biblioteca oRTP y libera los recursos utilizados.

### 2. Creación de puertos RTP
- **ortp_socket_create()**  
  Crea un socket RTP para recibir o enviar paquetes de audio.

- **ortp_socket_bind()**  
  Asocia un socket RTP a un puerto específico para recibir datos entrantes.

- **ortp_socket_send()**  
  Envía un paquete RTP a través de un socket ya configurado.

- **ortp_socket_recv()**  
  Recibe un paquete RTP de un socket configurado.

### 3. Manejo de los paquetes RTP
- **ortp_rtp_packet_new()**  
  Crea un nuevo paquete RTP para la transmisión de audio.

- **ortp_rtp_packet_set_payload()**  
  Establece la carga útil (datos de audio) en el paquete RTP.

- **ortp_rtp_packet_get_payload()**  
  Recupera los datos de audio de un paquete RTP.

### 4. Codificación y decodificación de audio
- **ortp_audio_encode()**  
  Codifica datos de audio para la transmisión mediante RTP.

- **ortp_audio_decode()**  
  Decodifica los datos de audio recibidos en un paquete RTP para su reproducción.

### 5. Sincronización y jitter
- **ortp_set_jitter_buffer()**  
  Establece el tamaño del buffer de jitter para compensar retrasos en la red.

- **ortp_set_timestamp_offset()**  
  Configura el desplazamiento de las marcas de tiempo en los paquetes RTP, facilitando la sincronización del audio.

### 6. Control de flujo y calidad
- **ortp_set_rtp_stats()**  
  Habilita la recolección de estadísticas RTP (pérdida de paquetes, jitter, latencia) para monitorear y optimizar la calidad de la llamada.

## Integración de Sofia-SIP y oRTP

La integración de estas dos bibliotecas permite gestionar de manera completa las llamadas SIP y la transmisión de audio:

1. **Establecimiento de la llamada**:  
   - Se inicia una sesión SIP con `sofia_create_session()`, negociando capacidades y codecs con el servidor SIP.
  
2. **Configuración del audio**:  
   - Se crea un socket RTP con `ortp_socket_create()` para manejar la transmisión de audio.

3. **Envío y recepción de audio**:  
   - **Envío**: Los datos de audio se codifican usando `ortp_audio_encode()` y se envían mediante `ortp_socket_send()`.
   - **Recepción**: Los paquetes RTP se reciben con `ortp_socket_recv()`, se decodifican con `ortp_audio_decode()` y se reproducen a través de la salida de audio (por ejemplo, usando PulseAudio o ALSA).

4. **Negociación de codecs**:  
   - Durante el establecimiento de la llamada, `sofia_set_codec()` se utiliza para acordar el codec que se usará. Posteriormente, oRTP se encarga de la codificación y decodificación de audio en tiempo real.

## Conclusión

Mediante la integración de **Sofia-SIP** y **oRTP**, es posible desarrollar aplicaciones de comunicación que gestionen llamadas SIP y transmitan audio en tiempo real. Las funciones detalladas permiten inicializar y finalizar sesiones, gestionar llamadas, enviar y recibir mensajes SIP, controlar la transmisión de audio y garantizar la calidad y sincronización del flujo de datos. Esto resulta fundamental para crear soluciones de VoIP robustas y eficientes.
