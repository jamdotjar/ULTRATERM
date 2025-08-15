# ULTRATERM
![Hero image](gallery/key_preview.png)

ULTRATERM is a miniature replica of an ULTRAKILL settings terminal ( like the one used in cybergrind ), that acts as a bluetooth audio player.

> ***NOTE**: this project is a WIP, check out the [journal](JOURNAL.md) for progress updates! Right now I'm:* `waiting for parts`

## Design

The terminal sits at around 125mm tall, with a 57x104mm footprint. It was designed to look cool, be fairly accurate, and act as a (mostly) stand alone device. It features a:
- 2.8" TFT display
- Touchscreen + 3 physical buttons
- A 2000mAh battery
- Bluetooth headphone support
- USB-C storage interface
- Removable SD card storage ( and a custom microSD card adapter )
The case is designed to be entirely 3D printable, and assembled without adhesive or fasteners.
While the terminal isn't a perfect replica, it's decently close. ( for example, there's none of the wires near the top, but there is a magnetic attachment point so I *can* add them )

This project was made as mostly a fun intro to custom hardware. I've made keyboards before, but design wise those are fairly simplec ( this isn't much more complicated ). I decided to model it after the ULTRAKILL Cybergrind terminal because that's the one that handles  cybergrind music ingame ( and because its form factor is also a *lot* more forgiving to fit everything into )

If you happen to try and make your own, I'd suggest either 3d-printing it in blue for minimum effort, or printing it in grey, painting it, then scuffing the paint to give it the worn look it has in-game. ( also check out the assembly section )

I'm still hacking away at getting everything working, but here's a video demo of my progress so far:

<iframe width="560" height="315" src="https://www.youtube.com/embed/Mtm72WqqxwA?si=b4hcNI8-wLqygqw2" title="YouTube video player" frameborder="0" allow="encrypted-media; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

## Gallery

![Assebled on tan bg](gallery/builtbg.png)
![ULTRATERM back IO render product render on black background](gallery/bl-back.png)
![ULTRATERM product render in ULTRAKILL enviroment](gallery/header.PNG)
![ULTRATERM left angle product render on black background](gallery/bl-ang-left.png)
*more in [./gallery](gallery)*

## PCB
<img width="875" alt="image" src="https://github.com/user-attachments/assets/9fd5e89d-9d3f-4c49-aeb6-2983ae4dbfe5" />
<img width="400" alt="Screenshot 2025-07-03 at 11 37 43 PM" src="https://github.com/user-attachments/assets/2922b129-de5d-4397-ad61-25974096de33" />
<img width="400" alt="Screenshot 2025-07-03 at 11 37 01 PM" src="https://github.com/user-attachments/assets/cc42b526-2fa3-4005-8990-93430f955d95" />
<img width="700" alt="image" src="https://github.com/user-attachments/assets/86971548-41dc-4e53-9309-176ed50dba61" />

## Case
<img width="380" alt="Screenshot 2025-07-03 at 11 41 00 PM" src="https://github.com/user-attachments/assets/c7ed8e2f-58ae-4164-a7fb-76fb8261c662" />
<img width="380" alt="Screenshot 2025-07-03 at 11 40 50 PM" src="https://github.com/user-attachments/assets/8aa4eb87-1eaa-4392-a68d-9a0aa4ad6cea" />

<img width="380" alt="Screenshot 2025-07-03 at 11 45 09 PM" src="https://github.com/user-attachments/assets/552af0e2-f642-4052-b81a-d14f586bfd60" />
<img width="380" alt="Screenshot 2025-07-03 at 11 44 03 PM" src="https://github.com/user-attachments/assets/f08572af-d590-4923-b969-a47b4e9e34b6" />

### Wiring

![image](https://github.com/user-attachments/assets/18c00dd0-d2a3-4937-aa91-c0b85836bf9b)

## BOM 
*prices in usd, see [bom.csv](/BOM.CSV) too*
|Item                                        |Quantity|Price|Supplier  |Notes                                                             |link                                                                                                      |
|--------------------------------------------|--------|-----|----------|------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------|
|PCB                                         |5       |4.7  |JLCPCB    |                                                                  |https://jlcpcb.com/                                                                                       |
|TINYPICO v2                                 |1       |20 | Digikey   |(+20 dollar shipping, ik its really expensive, I csn cover a good bit of the cost )|https://www.mouser.com/ProductDetail/392-TP-01                      |
|TZT 240x320 2.8" SPI ST7789V LCD Touch Panel|1       |7.09 |Aliexpress|                                                                  |https://www.aliexpress.com/item/1005004557916570.html                                                     |
|3.7v 2000mah LiPo battery                   |1       |16.92|Amazon    |price comparable to ali                                           |https://www.amazon.ca/Rechargeable-Controller-CUH-ZCT1-Electronic-Bluetooth/dp/B0CG178SHB                 |
|Pin Headers                                 |20      |2.00 |Aliexpress|for stupid LCD mounting soln.                                     |https://www.aliexpress.com/item/4000873858801                                                             |
|22Awg wire                                  |5m      |2.4  |Aliexpress|front panel mount + battery backup soln                           |https://www.aliexpress.com/item/1005004712828392.html                                                     |
|32gb microSD card                           |2       |4.7  |LCSC      |second waives 3$ fee, saves 1 dollar on order total               |https://lcsc.com/product-detail/Storage-products_SDTRUVAL-SDSDQAD-016G-SDWCK_C42416577.html               |
|32gb SD card                                |1       |5.8  |LCSC      |                                                                  |https://lcsc.com/product-detail/Storage-products_MK-MKSD032G-CGT1A_C22374946.html                         |
|LM809                                       |1       |0.66 |LCSC      |                                                                  |https://lcsc.com/product-detail/Supervisor-and-Reset-ICs_Texas-Instruments-LM809M3X-4-38-NOPB_C544520.html|
|JST-PH-2                                    |10      |0.59 |LCSC      |                                                                  |https://lcsc.com/product-detail/Wire-To-Board-Connector_JST-B2B-PH-K-K-LF-SN_C265438.html                 |
|SPDT slide switch                           |20      |0.48 |LCSC      |                                                                  |https://lcsc.com/product-detail/Slide-Switches_SHOU-HAN-SK12D07VG4_C393937.html                           |
|6mm pushbutton                              |50      |0.84 |LCSC      |                                                                  |https://lcsc.com/product-detail/Tactile-Switches_SHOU-HAN-TS665CJ_C393938.html                            |
|Amphenol 10067847-001RLF                    |1       |1.31 |LCSC      |                                                                  |https://lcsc.com/product-detail/SD-Card-Memory-Card-Connector_Amphenol-ICC-10067847-001RLF_C569097.html   |
|Hirose C719027                              |1       |1.04 |LCSC      |                                                                  |https://lcsc.com/product-detail/SD-Card-Memory-Card-Connector_HRS-Hirose-DM3D-SF_C719027.html             |
|LCSC Shipping                               |        |12.88|LCSC      |                                                                  |                                                                                                          |
|Total                                       |1       |95.21|          |                                                                  |                                                                                                          |
## Assembly

Putting this together is relatively simple, the hardest part is connecting the screen and front panel before putting the two halves together. Assuming you aren't too rough, the mounting holes for the screen and button panels should be enough to hold them. A small dab of superglue can help make sure they don't come out with repeated use. I'd advise dupont cables or soemthing similar to connect the screen, as long as it has a lot of slack it should be fine ( the case has a lot of empty space to wind the cables into )
![Assebled halfway](gallery/assembly.jpeg)

## LICENSE

This project uses the [CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/) for all firmware, documentation, renders, and other writing. Hardware including all Case and PCB designs, is licensed under the [CERN-OHL-W v2](https://ohwr.org/cern_ohl_w_v2.txt)
> This project was heavily inspired by the game ULTRAKILL, created by Hakita (Arsi "Hakita" Patala). The ULTRATERM design, as well as some assets used in renders are derived from or inspired by ULTRAKILL.
All rights to ULTRAKILL belong to Hakita and New Blood Interactive. This project is not affiliated with or endorsed by Hakita or New Blood. 
( [ULTRAKILL is a banger game, you should totally get it, it's like 20 bucks ](http://devilmayquake.com))

This project was made for [*highway*](https://highway.hackclub.com)
