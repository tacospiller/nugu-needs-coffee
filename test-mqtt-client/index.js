var mqtt = require("mqtt");

var client = mqtt.connect("", 
{
    host: "endpoint.iot.region.amazonaws.com",
    port: 8883,
    protocol: 'mqtt',
    clientId: "home-coffee",
    key: 
`-----BEGIN RSA PRIVATE KEY-----
-----END RSA PRIVATE KEY-----
`,
    cert: 
`-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
`,
    ca: 
`-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
`
});

client.on("message", (topic, buffer, packet) => {
    console.log(buffer.toString());
})

client.on("connect", (packet) => {
    client.subscribe("coffee/start");
})

client.on("error", (packet) => {
    console.log("error " + packet);
})
