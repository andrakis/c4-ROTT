// menuwalk.mjs [--no-accel] [SIZE] -- test-rott's walk through Extended User
// Options' RESOLUTION (rt_menu.c), from a c4 tree. ROTT on the bare machine
// with a display, started at SIZE (320x200 when none): Down until the main
// menu shows (its title, drawn at 320x200 at every size, MAINMENU), the
// menus by their letters (N, O, Enter, E, Enter, R, Enter), where the
// cursor starts on the size in use, then Up and Enter seven times,
// round the list past its top, and Down and Enter seven times, past its
// bottom. Each Enter switches the game to the size under the cursor, and
// rt_main.c prints it ("RESOLUTION: WxH"): those must be the sizes above
// and below, in turn, from every size, at the sizes whose menus the
// display scales (640x480, 800x600) as at 320x200. Prints what it saw and
// exits 1 when that is not it.
import { readFileSync, readdirSync, statSync } from 'node:fs';
import { join } from 'node:path';
const C4 = process.cwd();
const { createMachine, boot } = await import(join(C4, 'libjs/boot.js'));
const { GuiDevice, CMD } = await import(join(C4, 'libjs/gui-device.js'));
const SIZES = ['320x200', '320x240', '400x300', '512x384', '640x400', '640x480', '800x600'];
const argv = process.argv.slice(2);
const noAccel = argv.includes('--no-accel');
const start = argv.find(a => !a.startsWith('--')) || '320x200';
if (!SIZES.includes(start)) { console.log(`menuwalk: ${start} is not one of the sizes`); process.exit(2); }
if (!noAccel) { const { loadAccel, accelManifest } = await import(join(C4, 'libjs/accel.js')); await loadAccel(await accelManifest()); }
const G = join(C4, 'src/c4bb/images/games');
const files = new Map();
for (const n of readdirSync(G)) if (statSync(join(G, n)).isFile()) files.set(n, new Uint8Array(readFileSync(join(G, n))));
const gui = new GuiDevice({ w: 640, h: 480 });
const out = [];
const vm = createMachine({ arenaMb: 64, gui, drives: [{ files, writable: false, sink: null }], onByte: b => out.push(b), accel: !noAccel });
boot(vm, files.get('rott.c4r'), ['rott.c4r', 'NOSOUND', 'RESOLUTION', start]);
let nframes = 0, sized = null, halted = false, last = null;
// the main menu's title, a hash of the 320x200 frame's top middle: the
// credits and a demo before it take keys or not as their timing has it
const MAINMENU = 0xab7eaefc;
const title = () => {
  if (!last || last[2] !== 320 || last[3] !== 200) return 0;
  const px = last.subarray(6);
  let h = 0;
  for (let y = 4; y < 26; y++) for (let x = 96; x < 224; x++) h = (Math.imul(h, 31) + px[y * 320 + x]) | 0;
  return h >>> 0;
};
const pump = () => {
  const ws = gui.drain();
  for (let i = 0; ws && i < ws.length; i += ws[i]) {
    if (ws[i + 1] === CMD.RGB) { nframes++; last = ws.slice(i + 2, i + ws[i]); }
    if (ws[i + 1] === CMD.SIZE) sized = { w: ws[i + 2], h: ws[i + 3] };
  }
  if (sized && (sized.w !== gui.w || sized.h !== gui.h)) { gui.event({ kind: 'resize', ...sized }); sized = null; }
};
const run = n => { for (let k = 0; k < n && !halted; k++) { vm.run(2e6); if (vm.dev && vm.dev.halted) halted = true; pump(); } };
// a key held until two frames are shown (the menus read it between them),
// then let go and given time
const key = code => {
  gui.event({ kind: 'keydown', code, char: 0, mods: 0 });
  const f0 = nframes;
  for (let k = 0; k < 60 && nframes < f0 + 2; k++) run(1);
  gui.event({ kind: 'keyup', code, char: 0, mods: 0 });
  run(60);
};
const UP = 38, DOWN = 40, ENTER = 13;
run(600);
for (let t = 0; t < 30 && title() !== MAINMENU && !halted; t++) key(DOWN);
if (title() !== MAINMENU) { console.log(`menuwalk: from ${start}: no main menu`); process.exit(1); }
for (const k of [78, 79, ENTER, 69, ENTER, 82, ENTER]) key(k);
for (let i = 0; i < 7; i++) { key(UP); key(ENTER); }
for (let i = 0; i < 7; i++) { key(DOWN); key(ENTER); }
const seen = Buffer.from(out).toString('latin1').split('\n').filter(l => l.startsWith('RESOLUTION: ')).map(l => l.slice(12).trim());
const s = SIZES.indexOf(start), want = [];
for (let i = 1; i <= 7; i++) want.push(SIZES[(s - i + 7) % 7]);
for (let i = 1; i <= 7; i++) want.push(SIZES[(s + i) % 7]);
const ok = seen.join(' ') === want.join(' ');
console.log(`menuwalk: from ${start}${ok ? ', up and down through every size: OK' : ''}`);
if (!ok) { console.log(`  wanted ${want.join(' ')}\n  saw    ${seen.join(' ') || 'none'}${halted ? ' (the game ended)' : ''}`); process.exit(1); }
