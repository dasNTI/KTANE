const express = require('express');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const bodyParser = require('body-parser');

let serial = new SerialPort({
    baudRate: 9600,
    path: 'COM3'
});
const parser = new ReadlineParser();
serial.pipe(parser);
parser.on('data', (data) => console.log(data));

SerialPort.list().then(list => {
    serial.path = list[0];
});

const app = express();
app.use(express.json());
app.use(bodyParser.urlencoded());   

app.get('/', (req, res) => {
    res.sendFile(__dirname + '/index.html');
});


var currentSetup = {
    wires: false,
    keypads: false,
    simonSays: false,
    password: false,
    labyrinth: false,
    morse: false
}

var serialNr = "";
var serialNrLastDigitOdd;
var serialNrVowel;
function handleSerialNr() {
    let serialPossible = [...'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789']
    while (!/\d/.test(serialNr)) {
        serialNr = "";
        for (let i = 0; i < 6; i++) {
            let index = Math.floor(Math.random() * serialPossible.length)
            serialNr += serialPossible[index];
        }
    }

    let lastDigit = serialNr.split('').filter(d => /\d/.test(d)).pop() * 1;
    serialNrLastDigitOdd = lastDigit % 2 == 1;

    serialNrVowel = /[AEIOU]/.test(serialNr);
}
handleSerialNr();

app.post('/start', (req, res) => {
    let text = "G";
    let modules = req.body;

    let time = req.query['time'] * 2;
    if (String(time).length == 1) time = '0' + time;
    text += time + '_';

    if (modules.wires) {
        let wireSolution = null;
        function filterW(color, polarity) {
            return modules.wires.filter(w => (w == color) == polarity);
        }
        function lastWireIndexOf(color, polarity) {
            for (let i = modules.wires.length; i > 0; i--) {
                if ((modules.wires[i] == color) == polarity) return i;
            }
        }
        function certainNonEmptyWire(index) {
            let map = modules.wires.map((v, i) => {
                return {c: v, i: i}}
            );

            if (index == -1) return map.filter(w => w.c != 0).pop();
            return map.filter(w => w.c != 0)[index];
        }

        switch (filterW(0, false).length) {
            case 3:
                if (filterW(1, true).length == 0) {
                    wireSolution = certainNonEmptyWire(1).i;
                    break;
                }
                if (certainNonEmptyWire(-1).c == 5) {
                    wireSolution = lastWireIndexOf(5, true);
                    break;
                }
                if (filterW(3, true).length > 1) {
                    wireSolution = lastWireIndexOf(3, true);
                    break;
                }
                wireSolution = certainNonEmptyWire(-1).i;
            break;
            case 4:
                if (filterW(1, true).length > 1 && serialNrLastDigitOdd) {
                    wireSolution = lastWireIndexOf(1, true);
                    console.log('test');
                    break;
                }
                if (certainNonEmptyWire(-1).c == 2 && filterW(1, true).length == 0) {
                    wireSolution = certainNonEmptyWire(0).i;
                    break;
                }  
                if (filterW(3, true).length == 1) {
                    wireSolution = certainNonEmptyWire(0).i;
                    break;
                }
                if (filterW(2, true).length > 1) {
                    wireSolution = certainNonEmptyWire(-1).i;
                    break;
                }
                wireSolution = certainNonEmptyWire(0).i;
            break;
            case 5:
                if (certainNonEmptyWire(-1).c == 4 && serialNrLastDigitOdd) {
                    wireSolution = certainNonEmptyWire(3).i;
                    break;
                }
                if (filterW(1, true).length == 1 && filterW(2, true).length > 1) {
                    wireSolution = certainNonEmptyWire(0).i;
                    break;
                }
                if (filterW(4, true).length == 0) {
                    wireSolution = certainNonEmptyWire(1).i;
                    break;
                }
                wireSolution = certainNonEmptyWire(0).i;
            break;
            case 6:
                if (filterW(2, true).length == 0 && serialNrLastDigitOdd) {
                    wireSolution = certainNonEmptyWire(2).i;
                    break;
                }
                if (filterW(2, true).length == 1 && filterW(5, true) > 1) {
                    wireSolution = certainNonEmptyWire(3).i;
                    break;
                }
                if (filterW(1, true) == 0) {
                    wireSolution = certainNonEmptyWire(-1).i;
                    break;
                }
                wireSolution = certainNonEmptyWire(3).i;
            break;
        }
        
        text += 'W' + wireSolution + '_';
    }

    if (modules.keypads) {
        const lists = [
            [28, 13, 30, 12, 7, 9, 23],
            [16, 28, 23, 26, 3, 9, 20],
            [1, 8, 26, 5, 15, 30, 3],
            [11, 21, 31, 7, 5, 20, 4],
            [24, 4, 31, 22, 21, 19, 2],
            [11, 16, 27, 14, 24, 18, 6]
        ];

        let listIndex = modules.keypads.list;

        let map = modules.keypads.pads.map((v, i) => {
            return {
                btn: i,
                img: v,
                index: lists[listIndex].indexOf(v)
            };
        })
        let order = map.sort((a, b) => a.index - b.index);

        text += 'K';
        order.forEach(b => {
            text += b.btn;
        });
        text += '_';

        console.log(text);
    }

    if (modules.simonSays) {
        text += "S";

        let sequenceLength = 3 + Math.floor(Math.random() * 2);
        const colors = [0, 1, 2, 3]; // Blue, Yellow, Red, Green
        let mistake0Map, mistake1Map, mistake2Map;
        if (serialNrVowel) {
            mistake0Map = [2, 3, 0, 1];
            mistake1Map = [3, 2, 1, 0];
            mistake2Map = [2, 0, 3, 1];
        }else {
            mistake0Map = [1, 2, 0, 3];
            mistake1Map = [0, 3, 2, 1];
            mistake2Map = [3, 2, 1, 0];
        }

        let originalSequence = [];
        let mistake0Sequence = [], mistake1Sequence = [], mistake2Sequence = [];
        for (let i = 0; i < sequenceLength; i++) {
            let index = colors[Math.floor(Math.random() * 4)];
            originalSequence.push(colors[index]);
            mistake0Sequence.push(mistake0Map[index]);
            mistake1Sequence.push(mistake1Map[index]);
            mistake2Sequence.push(mistake2Map[index]);
        }

        text += originalSequence.join('') + ',';
        text += mistake0Sequence.join('') + ',';
        text += mistake1Sequence.join('') + ',';
        text += mistake2Sequence.join('') + '_';
    }
    
    if (modules.password) {
        text += "P";

        const possibleWords = [
            "ANGST", "ATMEN",
            "BETEN", "BOMBE",
            "DANKE", "DRAHT",
            "DRUCK", "DRÜCK",
            "FARBE", "FEHLT",
            "FERSE", "KABEL",
            "KNALL", "KNAPP",
            "KNOPF", "LEERE",
            "LEGAL", "LEHRE",
            "MATHE", "MATTE",
            "PANIK", "PIEPS",
            "RAUCH", "RUHIG",
            "SAITE", "SEHNE",
            "SEITE", "SENDE",
            "STROM", "SUPER",
            "TIMER", "ÜBRIG",
            "VERSE", "WARTE",
            "ZANGE"
        ];

        let charLists = [[], [], [], [], []];
        let word = possibleWords[Math.floor(Math.random() * possibleWords.length)];
        let chars = [...'ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ'];

        function charListsValid() {
            return possibleWords.find(w => w != word && charLists.reduce((prev, curr, index) => {
                if (!curr.indexOf(w.split('')[index])) prev = false;
            }, true));          
        }

        do {
            for (let i = 0; i < 5; i++) {
                do {
                    charLists[i] = [];
                    charLists[i].push(word.split('')[i]);
                    for (let j = 0; j < 4; j++) {
                        charLists[i].push(chars[Math.floor(Math.random() * chars.length)]);
                    }
                } while (charLists[i].find(l => charLists[i].filter(e => e == l).length > 1));
            }   
        } while (charListsValid());

        charLists.forEach(l => {
            l = l.sort((a, b) => Math.random() - 0.5);
        });

        text += word.split('').map((v, i) => charLists[i].indexOf(v)).join('');
        charLists.forEach(l => {
            text += ',' + l.join('');
        });
        text += '_';
    }

    if (modules.labyrinth) {
        text += 'L';
        text += modules.labyrinth + ',';
        let player = [0, 0], goal = [0, 0];

        do {
            player = [
                Math.floor(Math.random() * 8),
                Math.floor(Math.random() * 8)
            ];
            goal = [
                Math.floor(Math.random() * 8),
                Math.floor(Math.random() * 8)
            ];
        } while (Math.abs(player[0] - goal[0]) < 2 || Math.abs(player[1] - goal[1]) < 2)

        text += player.join('') + ',';
        text += goal.join('') + '_';
    }

    if (modules.morse) {
        text += "M";

        const words = [
            'halle', 'hallo', 'lokal',
            'steak', 'stück', 'speck',
            'bistro', 'robust', 'säbel',
            'sülze', 'sektor', 'vektor',
            'bravo', 'kobra', 'bombe',
            'süden'
        ];
        const alphabet = {
            a: '01',
            b: '1000',
            c: '1010',
            d: '100',
            e: '0',
            f: '0010',
            g: '110',
            h: '0000',
            i: '00',
            j: '0111',
            k: '101',
            l: '0100',
            m: '11',
            n: '10',
            o: '111',
            p: '0110',
            q: '1101',
            r: '010',
            s: '000',
            t: '1',
            u: '001',
            v: '0001',
            w: '011',
            x: '1001',
            y: '1011',
            z: '1100',
            ä: '0101',
            ö: '1110',
            ü: '0011'
        };

        let index = Math.floor(Math.random() * words.length);
        let code = words[index].split('').map(v => alphabet[v]).join('-');

        text += index + ',' + code + '_';
    }

    serial.write(text);
    console.log(text);
    res.sendStatus(200);
    res.end();
});

app.get('/random', (req, res) => {
    const count = req.query['count'] * 1;

    currentSetup = {
        wires: false,
        keypads: false,
        simonSays: false,
        password: false,
        labyrinth: false,
        morse: false
    }
    for (let i = 0; i < count; i++) {
        let index = Math.floor(Math.random() * (6 - i));
        let key = Object.keys(currentSetup).filter(k => currentSetup[k] == false)[index];
        currentSetup[key] = true;
    }

    if (currentSetup.wires) {
        currentSetup.wires = [];
        while (currentSetup.wires.filter(v => v != 0).length < 3) {
            currentSetup.wires = [];
            for (let i = 0; i < 6; i++) {
                currentSetup.wires.push(Math.floor(Math.random() * 6));
            }
        }
    }

    if (currentSetup.keypads) {
        let lists = [
            [28, 13, 30, 12, 7, 9, 23],
            [16, 28, 23, 26, 3, 9, 20],
            [1, 4, 26, 5, 15, 30, 3],
            [11, 21, 31, 7, 5, 20, 4],
            [24, 4, 31, 22, 21, 19, 2],
            [11, 16, 27, 14, 24, 18, 6],
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
        currentSetup.keypads = {
            list: listIndex,
            pads: images
        };
    }
    
    if (currentSetup.labyrinth) {
        currentSetup.labyrinth = Math.floor(Math.random() * 8);
    }

    console.log(currentSetup);

    res.send(currentSetup);
    res.end();
});

app.use('/static', express.static(__dirname + '/static/'));

const server = require('http').createServer(app);
server.listen(3000, () => {
    console.log(serialNr);
    setTimeout(() => {
        serial.write('D' + serialNr + ";" + require('ip').address());
    }, 1000);
});