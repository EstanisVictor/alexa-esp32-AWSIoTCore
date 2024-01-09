#include <pgmspace.h>

#define SECRET
#define THINGNAME "esp"
#define APP_KEY "[KEY SINRIC PRO]"
#define APP_SECRET "[SECRET SINRIC PRO]"
#define TEMP_SENSOR_ID "[SENSOR ID SINRIC PRO]"
#define WIFI_SSID "[NOME WIFI]"
#define WIFI_PASS "[SENHA WIFI]"

const char WIFI_SSID_AWS[] = "[NOME WIFI]";            // change this
const char WIFI_PASSWORD_AWS[] = "[SENHA WIFI]";       // change this
const char AWS_IOT_ENDPOINT[] = "[SEU EDOINT DA AWS]"; // change this

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
CERTIFICADO CA DA AWS
-----END CERTIFICATE-----
)EOF";

// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
CERTIFICADO CRT DA AWS
-----END CERTIFICATE-----
)KEY";

// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY----
CHAVE PRIVADA DA AWS
-----END RSA PRIVATE KEY-----
)KEY";