
# Custom BDN9 Keymap
Customized keymap and macros on a rev1 BDN9 macropad.

![BDN9](docs/bdn9.png)

![BDN9 Mapping](docs/map.png)

### Setup / Build / Workflow
Setup QMK env, assuming the repo is dumped into `~/qmk_firmware`.

From the clone of this repo:

	cp -r keyboards ~/qmk_firmware/
	qmk compile -kb keebio/bdn9 -km vs-w

Output to flash: `~/qmk_firmware/keebio_bdn9_rev1_vs-w_proton_c.bin`