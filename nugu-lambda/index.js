const { IoTDataPlaneClient, PublishCommand } = require("@aws-sdk/client-iot-data-plane");
const iotClient = new IoTDataPlaneClient({ region: "ap-northeast-2" });

exports.handler = async (event) => {
    console.log(JSON.stringify(event));
    
    var responseBody = "OK";
    if (event.routeKey === "POST /TriggerCoffeeMaker") {
        responseBody = await makeCoffee(event);
    } else if (event.routeKey === "GET /health") {
        responseBody = health();
    }

    const response = {
        statusCode: 200,
        body: JSON.stringify(responseBody),
    };
    return response;
};

async function makeCoffee(event) {
    var cmd = new PublishCommand({ topic: "coffee/start", qos: 1 });
    var result = "OK";
    try {
        await iotClient.send(cmd);
    } catch (e) {
        console.log(e);
        result = "PublishError";
    }
    const requestBody = JSON.parse(event.body);
    const responseBody = {
        version: requestBody.version,
        resultCode: result,
        output: {}
    }
    return responseBody;
}

function health() {
    return "OK";
}