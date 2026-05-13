Detalles de Implementación Técnica
Orden de los bits (Big-Endian)
Para asegurar que el receptor interprete correctamente, los bits se enviarán en este orden:

Header: Pulso de sincronización.

Dirección (MSB first): Quien envía.

Tipo: Comando o Respuesta.

Payload: R (8), G (8), B (8).

Checksum: Suma de los anteriores.

Validación de Datos
El receptor (Esclavo) realizará una interrupción por flanco de bajada. Al recibir los 48 bits, sumará los primeros 40 y los comparará con los últimos 8 (Checksum).

Si coincide: Actualiza su hardware PWM y emite una trama tipo 0x02 (ACK) con los mismos valores RGB.

Si no coincide: Ignora la trama. El maestro, al no recibir el ACK en el tiempo de ventana, no cambia su color local, manteniendo la coherencia del sistema.