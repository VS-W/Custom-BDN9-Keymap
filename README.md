
# Custom BDN9 Keymap
Customized keymap and macros on a rev1 BDN9 macropad.



### (for my own reference)
Setup QMK env, assuming the repo is dumped into `~/qmk_firmware`.

From the clone of this repo:

	cp -r keyboards ~/qmk_firmware/
	qmk compile -kb keebio/bdn9 -km vs-w

Output to flash: `~/qmk_firmware/keebio_bdn9_rev1_vs-w_proton_c.bin`