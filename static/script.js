"use strict";

let activeModules = {
    wires: true,
    keypads: true,
    simonSays: true,
    password: true,
    labyrinth: true,
    morse: true
};
let currentWires = [5, 2, 2, 0, 2, 4];
let currentKeypads = {};
let currentLabyrinth = 0;

$('#fullscreen').click(() => {
    if (window.innerHeight == screen.height) {
        document.exitFullscreen();
    }else {
        document.body.requestFullscreen();
    }
});

fetch(location.origin + '/srnr').then(res => {
    res.text().then(t => {
        $('[data-srnr]').html(t);
    });
});

$('.activator').each((i, input) => {
    $(input).change(() => {
        const module = input.id.replace('activate-', '');
        activeModules[module] = input.checked;
    });
});


// Wires
$('.wires-wire-color').each((i, select) => {
    const wireColors = [
        'transparent', 
        'red', 
        'yellow', 
        'blue', 
        'black', 
        'white'
    ];

    $(select).change(() => {
        $('.wires-wire').eq(i).css('--wire-color', wireColors[select.value]);

        if (currentWires.filter(w => w == 0).length == 3) {
            $('.wires-wire-color').each((i, select) => {
                $(select).removeClass('locked');
            });
        }

        currentWires[i] = Number(select.value);
        activeModules.wires = currentWires;

        if (currentWires.filter(w => w == 0).length == 3) {
            $('.wires-wire-color').each((i, select) => {
                if (select.value != 0) $(select).addClass('locked');
            });
        }
    });
});

// Keypads
$('#keypads-renew').click(() => {
    const lists = [
        [28, 13, 30, 12, 7, 9, 23],
        [16, 28, 23, 26, 3, 9, 20],
        [1, 8, 26, 5, 15, 30, 3],
        [11, 21, 31, 7, 5, 20, 4],
        [24, 4, 31, 22, 21, 19, 2],
        [11, 16, 27, 14, 24, 18, 6]
    ];

    let listIndex = Math.floor(Math.random() * lists.length);
    let list = lists[listIndex];

    let images = [];
    while (images.find(i => images.filter(m => m == i).length > 1) || images.length < 4) {
        images = [];
        for (let i = 0; i < 4; i++) {
            let index = Math.floor(Math.random() * list.length);
            images.push(list[index]);
        }
    }

    $('.keypads-pad img').each((i, img) => {
        img.src = location.origin + `/static/img/keypads/${images[i]}.png`;
    });
    currentKeypads = {
        list: listIndex,
        pads: images
    };
    activeModules.keypads = {
        list: listIndex,
        pads: images
    };
});

// Labyrinth

let maps = [
    [[0, 4], [5, 5]],
    [[0, 7], [6, 2]],
    [[2, 6], [4, 3]],
    [[3, 7], [5, 5]],
    [[2, 1], [5, 4]],
    [[6, 2], [3, 1]],
    [[0, 5], [7, 0]],
    [[2, 7], [6, 1]],
    [[2, 2], [4, 7]]
];

function drawLabyrinth(circles) {
    const canvas = $('canvas').get(0);
    const ctx = canvas.getContext('2d');

    canvas.height = canvas.offsetHeight * 2;
    canvas.width = canvas.offsetWidth * 2;

    ctx.fillStyle = 'black';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    for (let i = 0; i < 8; i++) {
        for (let j = 0; j < 8; j++) {
            let x = canvas.width * (i / 8 + 1 / 16);
            let y = canvas.height * (j / 8 + 1 / 16);
            let radius = canvas.height / 16 * 0.6;

            ctx.beginPath();
            ctx.arc(x, y, radius, 0, 2 * Math.PI, false);
            ctx.fillStyle = 'white';
            ctx.fill();
        }
    }
    circles.forEach(circle => {
        let x = canvas.width * (circle[0] / 8 + 1 / 16);
        let y = canvas.height * (circle[1] / 8 + 1 / 16);
        let radius = canvas.height / 16 * 1;

        ctx.beginPath();
        ctx.lineWidth = 5;
        ctx.arc(x, y, radius, 0, 2 * Math.PI, false);
        ctx.strokeStyle = 'rgb(77, 175, 77)';
        ctx.stroke();
    })
}
drawLabyrinth(maps[currentLabyrinth]);

$('[data-labyrinth-btn-next]').click(() => {
    if (currentLabyrinth == 8) currentLabyrinth = -1;
    currentLabyrinth++;
    drawLabyrinth(maps[currentLabyrinth]);
    activeModules.labyrinth = currentLabyrinth;
});
$('[data-labyrinth-btn-back]').click(() => {
    if (currentLabyrinth == 0) currentLabyrinth = 8;
    currentLabyrinth--;
    drawLabyrinth(maps[currentLabyrinth]);
    activeModules.labyrinth = currentLabyrinth;
});

// General

$('[data-footer-clock-range]').on("input change", () => {
    let value = $('[data-footer-clock-range]')[0].value * 1;
    let min = String(Math.floor(value));
    let sec = String((value % 1) * 60);

    if (min.length < 2) min = '0' + min;
    if (sec.length < 2) sec = '0' + sec;

    $('[data-footer-clock]').html(`${min}:${sec}`);
});
$('[data-footer-module-range]').on("input change", () => {
    let value = $('[data-footer-module-range]')[0].value;
    $('[data-footer-module-count]').html(value);
});

async function randomModules(ani) {
    let count = $('[data-footer-module-range]')[0].value;

    let modules = await fetch(location.origin + "/random?count=" + count);
    activeModules = await modules.json();
    currentKeypads = activeModules.keypads;
    currentLabyrinth = activeModules.labyrinth;
    currentWires = activeModules.wires || [1, 2, 0, 0, 4, 0];

    $('.activator').each((i, checkbox) => {
        let value = activeModules[checkbox.id.replace('activate-', '')];
        checkbox.checked = Boolean(value);
    });

    if (activeModules.wires) {
        $('.wires-wire-color').each((i, select) => {
            $(select).children(`[value=${activeModules.wires[i]}]`).prop('selected', true);
            $(select).change();
        });
    }
    if (activeModules.keypads) {
        $('.keypads-pad').each((i, pad) => {
            $(pad).children()[0].src = `/static/img/keypads/${activeModules.keypads.pads[i]}.png`
        });
    }
    if (activeModules.labyrinth) {
        currentLabyrinth = activeModules.labyrinth;
        drawLabyrinth(maps[currentLabyrinth]);
    }

    if (!ani) return;
    $('#overlay').addClass('active');
    $('#overlay').one('animationend', () => {
        $('#overlay').removeClass('active');
    });
} 

$('[data-random-module-btn]').click(async () => {randomModules(true);});
randomModules(false);

$('[data-start-btn]').click(() => {
    if (activeModules.wires) activeModules.wires = currentWires;
    if (activeModules.keypads) activeModules.keypads = currentKeypads;
    if (activeModules.labyrinth) activeModules.labyrinth = currentLabyrinth;

    let body = Object(activeModules);
    console.log(body);

    fetch(location.origin + "/start?time=" + $('[data-footer-clock-range]')[0].value * 1, {
        method: 'POST',
        body: JSON.stringify(body),
        headers: {
            "Content-Type": "application/json"
        }
    }).then(() => {
        $('[data-start-btn]').html(
            $('[data-start-btn]').html() == 'Start' ? 'Reset' : 'Start'
        );
    });
});