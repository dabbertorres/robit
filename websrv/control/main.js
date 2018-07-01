window.addEventListener("load", function ()
{
    "use strict";

    let ws = new WebSocket("ws://" + window.location.host + "/ws");

    let setInput = key =>
    {
        switch (key)
        {
            case 'w':
            case 'ArrowUp':
                return "up";

            case 'a':
            case 'ArrowLeft':
                return "left";

            case 'd':
            case 'ArrowRight':
                return "right";

            case 's':
            case 'ArrowDown':
                return "down";

            case 'Space':
                return "stop";

            default:
                return;
        }
    };

    window.addEventListener("keydown", function (event)
    {
        let input = {
            input: setInput(event.key),
            press: true,
        };

        if (input.input !== "")
            ws.send(JSON.stringify(input));
    });

    window.addEventListener("keyup", function (event)
    {
        let input = {
            input: setInput(event.key),
            press: false,
        };

        if (input.input !== "")
            ws.send(JSON.stringify(input));
    });
});

