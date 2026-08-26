translate([-5,0.5,8])
rotate([90,0,0])
scale([1.02,1.02,.8])
import("din_rail_mount.stl");

$fn=30;

pcb_l = 100;
pcb_h = 70;
pcb_w = 2.0;

tray_w = pcb_w+4;
tray_l = pcb_l+ 6;
tray_h = 4 + 6;

module pcb(){
    translate([-pcb_l/2,0,0])
    cube([pcb_l, pcb_w, pcb_h]);
}
//pcb();

module pcb_grove(){
    difference(){
        union(){
            translate([-tray_l/2,0,0])
                cube([tray_l, tray_w, tray_h]);
            translate([-tray_l/2,0,3])
                cube([10, tray_w, tray_h]);
            translate([tray_l/2-12,0,2])
                cube([12, tray_w, tray_h]);
        }
        translate([0,2,4])
            pcb();
        
    }
    translate([-(50-4.5), 2+pcb_w+.7, 4+4+1.5])
        sphere(d=2.5);
    translate([(50-7), 2+pcb_w+.7, 4+4+1.5])
        sphere(d=2.5);
}

pcb_grove();