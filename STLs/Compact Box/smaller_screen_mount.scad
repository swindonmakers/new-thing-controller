//translate([-5,2,5])
//rotate([90,0,0])
//scale([1.06,1.06,1.06])
//import("din_rail_mount.stl");
$fn=20;
//lcd
lcd_hole_pitch_w = 75.9;
lcd_hole_pitch_l = 44;


mount_hole_w = 100;
mount_hole_d = 8.2;

rfid_hole_w = 42;
rfid_hole_d = 4;

mount_h = 2;
mount_l = 20;


full_h = 46 + 47; //box depth + lid depth
lcd_h = 5;
lcd_mount_h = 6; //6mm stand off (8mm for rdif as thinner)
arm_t = 3; //thickness of arms holding lcd and rfif
arm_l = 11;

stand_off_h = full_h - lcd_h - lcd_mount_h; //how tall it needs to be
stand_off_hp = full_h - lcd_h - lcd_mount_h-arm_t; //base of arm hieght


//basemountframe
difference(){
    union(){
        translate([-6,-6,0])
            cube([mount_hole_w+12,mount_l,mount_h]);
        translate([0,2,0])
        cylinder(h=6.5, d =mount_hole_d+4);
        
        translate([mount_hole_w,2,0])
            cylinder(h=6.5, d =mount_hole_d+4);
        
    }
    translate([0,2,0])
    cylinder(h=6.5, d =mount_hole_d);
    translate([mount_hole_w,2,0])
        cylinder(h=6.5, d =mount_hole_d);
    translate([mount_hole_w/2-10,-2,0])
        cube([20,10,mount_h]);   //screw mount is in the way
}

//upright bit
translate([-6,14,0])
difference(){
    cube([mount_hole_w+12,2,stand_off_h]);
    translate([8,0,16])
    cube([mount_hole_w-4,2,stand_off_h-30]);
}
//support
translate([-6,12,0])
cube([2,2,stand_off_h]);
translate([mount_hole_w +6-2,12,0])
cube([2,2,stand_off_h]);
#translate([-6,12,stand_off_hp])
cube([mount_hole_w+12,2,3]);


//arms to hold lcd and rfid
//rightmost arm
color("blue")
translate([mount_hole_w-arm_l/2,14-60,stand_off_hp])
difference(){
    cube([arm_l-2,60,arm_t]);
    translate([0,5,0]){
        translate([arm_l/2,0,0])
            #cylinder(h=6, d =rfid_hole_d);
        translate([arm_l/2,lcd_hole_pitch_l,0])
            #cylinder(h=6, d =rfid_hole_d);
    }
}
//left arm for lcd
color("blue")
translate([mount_hole_w-arm_l/2-lcd_hole_pitch_w,14-60,stand_off_hp])
difference(){
    cube([arm_l,60,arm_t]);
    translate([0,5,0]){
        translate([arm_l/2,0,0])
            #cylinder(h=6, d =rfid_hole_d);
        translate([arm_l/2,lcd_hole_pitch_l,0])
            #cylinder(h=6, d =rfid_hole_d);
    }
}

//rfid right arm
color("blue")
translate([mount_hole_w-arm_l/2-lcd_hole_pitch_w-18,14-60,stand_off_hp])
difference(){
    cube([arm_l,60,arm_t]);
    translate([0,5,0]){
        translate([arm_l/2,0,0])
            #cylinder(h=6, d =rfid_hole_d);
    }
}

//rfid left arm
color("blue")
translate([mount_hole_w-arm_l/2-lcd_hole_pitch_w-18-25+4,14,stand_off_hp])
difference(){
    translate([26,0,0])
    rotate([0,0,180])
    linear_extrude(arm_t)
    polygon([[0,0],[25,33],[32,28],[10,0]]);
    
    #translate([0,-(60-2-31),0])
        cylinder(h=6, d =rfid_hole_d);
    
    
}

module angle_supp(siz=5, wid=2) {
    rotate([0,-90,180])
    linear_extrude(wid)
    polygon([[0,0],[siz,0],[0,siz]]);
    
}
//supports
translate([-6,14,0])
angle_supp(15);

translate([mount_hole_w+4,14,0])
angle_supp(15);

translate([25,14,0])
angle_supp(15, 4);

translate([75,14,0])
angle_supp(15, 4);

translate([mount_hole_w+4,14,stand_off_hp])
rotate([0,180,0])
angle_supp(10);

translate([14,14,stand_off_hp])
rotate([0,180,0])
angle_supp(10,4);

translate([5,14,stand_off_hp])
rotate([0,180,0])
angle_supp(10,2);

translate([29,14,stand_off_hp])
rotate([0,180,0])
angle_supp(8,4);

translate([10,-4,stand_off_hp])
cube([10,20,arm_t]);