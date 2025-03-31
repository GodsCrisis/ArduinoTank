module arduino(length = 68.6,width = 53.4,height = 12.5){
    cube([length,width,height]);
}
module servo(length = 22,width = 27,height = 12) {
    cube([length,width,height]);
}
module power_bank(length = 147,width = 69,height = 19){
    cube([length,width,height]);
}
module motor(length = 26.3,width = 15.5,height = 12){
    cube([length,width,height]);
}
module usb_c(length = 24.5,width = 20.2,height = 4.9){
    cube([length,width,height]);
}
translate([70,0,19]) rotate(90) arduino(68.6,53.4,12.5);

translate([15,15,31.5]) servo();

translate([0,0,0]) power_bank();

translate([20,70,0]) motor();

translate([60,70,0]) motor();

translate([150,20,0]) usb_c();
