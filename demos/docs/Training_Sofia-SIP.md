# Sofia-SIP

## Descripción

Sofia-SIP es una pila SIP de código abierto robusta utilizada para la implementación de clientes y servidores SIP. Es compatible con los principales protocolos de señalización SIP y se usa ampliamente en telecomunicaciones y VoIP.

Este proyecto proporciona una serie de demos prácticas para aprender a utilizar Sofia-SIP desde un nivel básico hasta intermedio-avanzado.

## Demos

### **Demo 1: Instalación y configuración básica**

**Objetivo:** Instalar **Sofia-SIP** y configurar el entorno de desarrollo.

### Para instalar
   ```sh
>> sudo apt-get update
>> sudo apt install libsofia-sip-ua-dev
>> sudo apt install libsofia-sip-ua-glib-dev
>> sudo apt install libsofia-sip-ua-glib3
>> sudo apt install libsofia-sip-ua0
>> sudo apt install telepathy-sofiasip
   ```

### Para remover
   ```sh
>> sudo apt-get remove libsofia-sip-ua-dev
>> sudo apt-get purge libsofia-sip-ua-dev
>> sudo apt-get purge --auto-remove libsofia-sip-ua-dev
   ```

---

### **Demo 2: Crear un Cliente SIP Simple**

**Objetivo:** Enviar un mensaje SIP **REGISTER** a un servidor SIP.

1. Crear un script en **C** que use Sofia-SIP para registrar un usuario en un servidor SIP.
2. Utilizar `nta_agent_create()` para inicializar la pila SIP.
3. Enviar un mensaje **REGISTER** con un `Contact:` adecuado.
4. Capturar y analizar la respuesta del servidor.

#### Para compilar
   ```sh
>> gcc demo2.c -o demo2 $(pkg-config --cflags --libs sofia-sip-ua)
   ```

### **Demo 3: Implementación de INVITE**

**Objetivo:** Establecer una llamada SIP básica.

1. Enviar un **INVITE** a otro usuario SIP.
2. Manejar respuestas como **180 Ringing** y **200 OK**.
3. Implementar el manejo de **ACK**.
4. Usar **Wireshark** para analizar el tráfico SIP generado.

---

### **Demo 4: Manejo de Mensajes SIP (BYE, CANCEL)**

**Objetivo:** Terminar llamadas correctamente.

1. Modificar el código del **Demo 3** para incluir el manejo de **BYE**.
2. Implementar la cancelación de una llamada con **CANCEL** antes de ser aceptada.
3. Capturar y analizar los mensajes en Wireshark.

---

### **Demo 5: Implementación de Mensajería Instantánea SIP (MESSAGE)**

**Objetivo:** Implementar un cliente que envíe y reciba mensajes de texto.

1. Usar el método SIP **MESSAGE** para enviar texto entre dos usuarios SIP.
2. Implementar un pequeño servidor en C que reciba mensajes y responda con **202 Accepted**.
3. Agregar una interfaz básica en línea de comandos para enviar y recibir mensajes.

---

### **Demo 6: Manejo de Redirecciones (3XX)**

**Objetivo:** Gestionar respuestas de redirección.

1. Simular un escenario donde el servidor SIP responde con un **302 Moved Temporarily**.
2. Modificar el código para reenviar el **INVITE** a la nueva dirección.

---

### **Demo 7: Seguridad en SIP (Autenticación y TLS)**

**Objetivo:** Implementar autenticación y cifrado.

1. Configurar **Sofia-SIP** para manejar autenticación con **Digest Authentication**.
2. Probar una conexión con un servidor SIP que requiera usuario/contraseña.
3. Usar **TLS** para cifrar la comunicación.

---

### **Demo 8: Integración con RTP para Audio**

**Objetivo:** Añadir soporte de audio a las llamadas SIP.

1. Usar Sofia-SIP junto con una librería RTP (como **oRTP** o **PJSUA**) para enviar audio en llamadas SIP.
2. Implementar la negociación de codecs en **SDP**.
3. Realizar una llamada real y transmitir audio.

---

## Contribuidores

- **César M. Varela García** – QA & Desarrollador
- **Vanessa Lorenzo Toquero** – Desarrolladora & QA

## Licencia

Este proyecto ha sido desarrollado como parte del currículo de 42 y sigue sus normas de integridad académica. Úsalo y modifícalo bajo tu propia responsabilidad.

