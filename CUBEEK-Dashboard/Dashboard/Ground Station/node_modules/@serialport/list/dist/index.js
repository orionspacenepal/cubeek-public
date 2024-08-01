#!/usr/bin/env node
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const bindings_cpp_1 = require("@serialport/bindings-cpp");
const commander_1 = require("commander");
const { version } = require('../package.json');
const formatOption = new commander_1.Option('-f, --format <type>', 'Format the output').choices(['text', 'json', 'jsonline', 'jsonl']).default('text');
commander_1.program.version(version).description('List available serial ports').addOption(formatOption).parse(process.argv);
function jsonl(ports) {
    ports.forEach(port => {
        console.log(JSON.stringify(port));
    });
}
const formatters = {
    text(ports) {
        ports.forEach(port => {
            console.log(`${port.path}\t${port.pnpId || ''}\t${port.manufacturer || ''}`);
        });
    },
    json(ports) {
        console.log(JSON.stringify(ports));
    },
    jsonl,
    jsonline: jsonl,
};
const args = commander_1.program.opts();
(0, bindings_cpp_1.autoDetect)()
    .list()
    .then(ports => {
    const formatter = formatters[args.format];
    if (!formatter) {
        throw new Error(`Invalid formatter "${args.format}"`);
    }
    formatter(ports);
})
    .catch(err => {
    console.error(err);
    process.exit(1);
});
