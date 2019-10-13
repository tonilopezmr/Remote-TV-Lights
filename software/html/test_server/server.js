const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 80 });

wss.on('connection', function connection(ws) {
    var state = false;
    var cont = 0;
    ws.on('message', function incoming(message) {
        console.log('received: %s', message);
        if (message.includes('wifi')) {
            ws.send(JSON.stringify({ restart: true }));
        }
        state = !state;
        ws.send(JSON.stringify({ relay: state, boardName: "IR-TV-BOARD" }));
    });
    const intervalId = setInterval(function () {
        ws.send(JSON.stringify({ code: "code " + cont }));
        cont++;
        if (cont > 8) {
            clearInterval(intervalId);
        }
    }, 1000);
    ws.send(JSON.stringify({ relay: state, boardName: "IR-TV-BOARD" }));
});