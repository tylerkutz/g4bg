#include "g4rcDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "g4rcDetector.hh"
#include "g4rcMaterial.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polycone.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include "TString.h"

g4rcDetectorConstruction::g4rcDetectorConstruction() {

	fGEMCenter[0] = 229.99575 * cm;	

}


g4rcDetectorConstruction::~g4rcDetectorConstruction(){;}


G4VPhysicalVolume* g4rcDetectorConstruction::Construct() {
    

	SDman = G4SDManager::GetSDMpointer();
	fMaterial = g4rcMaterial::GetMaterialManager();

	G4double length = 20.0*cm;
	G4double radius = 0.5*cm;
	G4double window = 0.125*mm;
	G4double wall = 0.2*mm;

	// WORLD LOGICAL
	
	double world_x, world_y, world_z;

	world_x = world_y = world_z = 3*m;

	G4Box* world_box = new G4Box("world",world_x,world_y,world_z);

	G4LogicalVolume* world_log
	= new G4LogicalVolume(world_box,fMaterial->air,"World",0,0,0);

	world_log->SetVisAttributes(G4VisAttributes::Invisible);

	// TARGET CHAMBER

	double t_chamber = 0.406*mm;
	double r_chamber = (1037./2.)*mm;
	
	G4Tubs* target_mother_tubs = new G4Tubs("targ_mother_tubs", 0., r_chamber+t_chamber, 25.*cm, 0.*deg, 360.*deg);
	G4LogicalVolume* target_mother_log = new G4LogicalVolume(target_mother_tubs,fMaterial->vacuum,"target_mother_logical",0,0,0);
	target_mother_log->SetVisAttributes(G4VisAttributes::Invisible);

	G4RotationMatrix* rotX_neg90 = new G4RotationMatrix();
	rotX_neg90->rotateX(-90.*deg);
	G4RotationMatrix* rotX_pos90 = new G4RotationMatrix();
	rotX_pos90->rotateX(90.*deg);

	// Target cell

	G4Tubs* cell_tubs = new G4Tubs("cell_tubs", 0., radius + wall, length/2. + window, 0.*deg, 360.*deg);
	G4Tubs* cell_empt = new G4Tubs("cell_empt", 0., radius, length/2., 0.*deg, 360.*deg);

	G4SubtractionSolid* cell_sub = new G4SubtractionSolid("cell_sub", cell_tubs, cell_empt);
	G4LogicalVolume* cell_log = new G4LogicalVolume(cell_sub, fMaterial->aluminum, "cell_logical", 0, 0, 0);
	G4VPhysicalVolume* cell_phys = new G4PVPlacement(rotX_pos90,G4ThreeVector(), cell_log, "cell_physical", target_mother_log, false, 0);

	// Target gas volume
	
	G4Tubs* gas_tubs = new G4Tubs("gas_tubs", 0., radius, length/2., 0.*deg, 360.*deg);
//	G4LogicalVolume* gas_log = new G4LogicalVolume(gas_tubs, target_gas[fTargIndex], "gas_logical", 0,0,0);
	G4LogicalVolume* gas_log = new G4LogicalVolume(gas_tubs, fMaterial->D2_liquid, "gas_logical", 0,0,0);
	G4VisAttributes* gas_vis = new G4VisAttributes(G4Colour(0.,0.,1.));
	gas_log->SetVisAttributes(gas_vis);
	G4VPhysicalVolume* gas_phys = new G4PVPlacement(rotX_pos90,G4ThreeVector(), gas_log, "gas_physical", target_mother_log, false, 0);	


/*
	// Beryllium window (upstream of target)
	
	double t_be = 0.2003*mm;
	G4Tubs* be_tubs = new G4Tubs("be_tubs", 0., 5.*cm, t_be/2., 0.*deg, 360.*deg);
	G4LogicalVolume* be_log = new G4LogicalVolume(be_tubs, fMaterial->beryllium, "be_logical", 0,0,0);
	G4VisAttributes* be_vis = new G4VisAttributes(G4Colour(1.,1.,0.));
	be_log->SetVisAttributes(be_vis);
	G4VPhysicalVolume* be_phys = new G4PVPlacement(rotX_pos90,G4ThreeVector(0.,-30.*cm,0.), be_log, "be_physical", target_mother_log, false, 0);
*/
	
	G4VPhysicalVolume* target_mother_phys 
	= new G4PVPlacement(rotX_neg90,G4ThreeVector(), target_mother_log, "target_mother_physical", world_log, false, 0); 


	// Define the LAD GEMs
	double h_gem1 = 0.96*m;
	double w_gem1 = 0.44*m;
	double h_gem2 = 1.2*m;
	double w_gem2 = 0.55*m;
	
	double r_gem1 = 70.*cm;
	double r_gem2 = 90.*cm;
	double gem_angle = 127.*deg;

	double x_gem1 = r_gem1*sin(gem_angle);
	double z_gem1 = r_gem1*cos(gem_angle);

	double x_gem2 = r_gem2*sin(gem_angle);
	double z_gem2 = r_gem2*cos(gem_angle);

	G4ThreeVector pos1 = G4ThreeVector(x_gem1, 0., z_gem1);
	G4ThreeVector pos2 = G4ThreeVector(x_gem2, 0., z_gem2);

	G4RotationMatrix* rot_gem = new G4RotationMatrix();
	rot_gem->rotateY(180.*deg - gem_angle);

/*
	// A "dumb" GEM for debugging
	
	G4Box* gem1_box = new G4Box("gem1_box", w_gem1/2., h_gem1/2., 2.5*cm);  
	G4LogicalVolume* gem1_log = new G4LogicalVolume(gem1_box, fMaterial->vacuum, "gem1_log", 0, 0, 0);
	g4rcDetector* gem1_SD = new g4rcDetector("gem1_SD",501);
	SDman->AddNewDetector(gem1_SD);
	gem1_log->SetSensitiveDetector(gem1_SD);
	G4VPhysicalVolume* gem1_phys = new G4PVPlacement(rot_gem, G4ThreeVector(30.*cm, 0., -30.*cm), gem1_log, "gem1_physical", world_log, false, 0);
*/


	AddGEM(world_log, 101, false, 55.04*cm, 122.88*cm, rot_gem, pos1);
	AddGEM(world_log, 102, false, 55.04*cm, 122.88*cm, rot_gem, pos2);

	// Define the GMn GEMs
	
	double w_gmn = 1.*m;
	double h_gmn = 1.*m;

	double t_gmn = 0.5*cm;

	double r_gmn = 1.5*m;
	
	double gmn_angle = 40.*deg;

	double x_gmn = r_gmn*sin(gmn_angle);
        double z_gmn = r_gmn*cos(gmn_angle);
	
	G4ThreeVector pos_gmn = G4ThreeVector(x_gmn, 0., z_gmn);

	G4RotationMatrix* rot_gmn = new G4RotationMatrix();
        rot_gmn->rotateY(180.*deg - gmn_angle);

	AddGEM(world_log, 201, false, 50.*cm, 50.*cm, rot_gmn, pos_gmn); 	

	G4VPhysicalVolume* world_phys
	= new G4PVPlacement(0,G4ThreeVector(),world_log,"World",0,false,0);

	return world_phys;

}


void g4rcDetectorConstruction::AddGEM(G4LogicalVolume *mother, int layerid, bool culess, double GEMx, double GEMy, G4RotationMatrix* GEMRot, G4ThreeVector GEMPos) {

    G4Material *DefaultM = G4Material::GetMaterial("Galaxy");
    G4Material *GEMFrameM = G4Material::GetMaterial("NemaG10");
    G4Material *GEMGasM = G4Material::GetMaterial("ArCO2");
    G4Material *GEMFoilM = G4Material::GetMaterial("Kapton");
    G4Material *GEMFoil0d2M = G4Material::GetMaterial("Kapton0.2");
    G4Material *GEMFoil0d8M = G4Material::GetMaterial("Kapton0.8");
    G4Material *GEMCuM = G4Material::GetMaterial("Copper");
    G4Material *GEMCu0d2M = G4Material::GetMaterial("Copper0.2");
    G4Material *GEMCu0d75M = G4Material::GetMaterial("Copper0.75");
    G4Material *GEMCu0d8M = G4Material::GetMaterial("Copper0.8");
    G4Material *GEMGlueM = G4Material::GetMaterial("Kapton"); // TODO: Add actual Glue material

    // GEM
//    G4double GEMCenter = fGEMCenter[layerid];
    G4double GEMGap = 3.971 * cm; // Gap between two GEM // 3.971 from Weizhi
    G4double GEMHalfX = GEMx / 2.0;
    G4double GEMHalfY = GEMy / 2.0;
    G4double GEMHalfT = (15.0 * mm + 455.0 * um) / 2.0; // 2 * 25 + 5 + 50 (win) + 6 * 5 + 3 * 50 (foil) + 5 + 5 + 50 + 50 + 60 (readout)

    if (culess) GEMHalfT = (15.0 * mm + 410.0 * um) / 2.0; // 2 * 25 + 50 (win) + 3 * 50 (foil) + 50 + 50 + 60 (readout)

    G4double GEMSpacerWh = 0.3 * mm / 2.0;
    G4double GEMSpacerWv = 0.5 * mm / 2.0;
    G4double GEMSpacerT = (2.0 - 0.1) * mm;
    G4double GEMHoleR = 2.2 * cm;
    G4double GEMCenterHalfXY = 7.4 * cm / 2.0;
    G4double GEMFrameWidth = 1.5 * cm;
//    G4double GEMCenterOffset = GEMHalfX + GEMFrameWidth - GEMCenterHalfXY;
    G4double GEMCenterOffset = 0.;

    // GEM Container
    G4Box *solidGEMCon = new G4Box(Form("GEM%dConBox", layerid), 1.0 * m, 1.0 * m, (GEMGap + 2.0 * GEMHalfT + 1.0 * mm) / 2.0);
//    G4Tubs *GEMConTube = new G4Tubs(Form("GEM%dConTube", layerid), 0, GEMHoleR, (GEMGap + 2.0 * GEMHalfT + 1.0 * mm) / 2.0 + 0.1 * mm, 0, twopi);
//    G4SubtractionSolid *solidGEMCon = new G4SubtractionSolid(Form("GEM%dContainerS", layerid), GEMConBox, GEMConTube);
    G4LogicalVolume *logicGEMCon = new G4LogicalVolume(solidGEMCon, DefaultM, Form("GEM%dContainerLV", layerid));
    logicGEMCon->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(GEMRot, GEMPos, logicGEMCon, Form("GEM %d Container", layerid), mother, false, 2 * layerid);

    // GEM
    G4Box *solidGEM = new G4Box(Form("GEM%dBox", layerid), GEMHalfX + GEMFrameWidth, GEMHalfY + GEMFrameWidth * 2.0, GEMHalfT);
//    G4Tubs *GEMTube = new G4Tubs(Form("GEM%dTube", layerid), 0, GEMHoleR, GEMHalfT + 0.1 * mm, 0, twopi);
//    G4SubtractionSolid *solidGEM = new G4SubtractionSolid(Form("GEM%dS", layerid), GEMBox, GEMTube, 0, G4ThreeVector(-GEMCenterOffset, 0, 0));
    G4LogicalVolume *logicGEM = new G4LogicalVolume(solidGEM, DefaultM, Form("GEM%dLV", layerid));
    logicGEM->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(0, G4ThreeVector(GEMCenterOffset, 0, GEMGap / 2.0), logicGEM, Form("GEM %d L", layerid), logicGEMCon, false, 0);

//    G4RotationMatrix rmGEM;
//    rmGEM.rotateZ(180.0 * deg);
//    new G4PVPlacement(G4Transform3D(rmGEM, G4ThreeVector(-GEMCenterOffset, 0, -GEMGap / 2.0)), logicGEM, Form("GEM %d R", layerid), logicGEMCon, false, 1);

    // GEM Gas
    G4Box *solidGEMGas = new G4Box(Form("GEM%dGasBox", layerid), GEMHalfX, GEMHalfY, GEMHalfT);
//    G4Box *GEMSubBox = new G4Box(Form("GEM%dSubBox", layerid), GEMCenterHalfXY, GEMCenterHalfXY, GEMHalfT + 0.1 * mm);
//    G4SubtractionSolid *solidGEMGas = new G4SubtractionSolid(Form("GEM%dGasS", layerid), GEMGasBox, GEMSubBox, 0, G4ThreeVector(-GEMCenterOffset, 0, 0));
    G4LogicalVolume *logicGEMGas = new G4LogicalVolume(solidGEMGas, GEMGasM, Form("GEM%dGasLV", layerid));

	g4rcDetector* GEM_SD = new g4rcDetector(Form("GEM_SD_%i", layerid),layerid);
	SDman->AddNewDetector(GEM_SD);
	logicGEMGas->SetSensitiveDetector(GEM_SD);

	G4VisAttributes* GEM_gas_vis = new G4VisAttributes(G4Colour(0.5,0.,1.));
	logicGEMGas->SetVisAttributes(GEM_gas_vis);
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicGEMGas, Form("GEM %d Gas", layerid), logicGEM, false, 0);

    // GEM Frame
    G4Box *GEMFrameBox1 = new G4Box(Form("GEM%dFrameBox1", layerid), GEMHalfX + GEMFrameWidth, GEMHalfY + GEMFrameWidth * 2.0, GEMHalfT);
    G4Box *GEMFrameBox2 = new G4Box(Form("GEM%dFrameBox2", layerid), GEMHalfX, GEMHalfY, GEMHalfT + 0.1 * mm);
    G4SubtractionSolid *solidGEMFrame = new G4SubtractionSolid(Form("GEM%dFrameS", layerid), GEMFrameBox1, GEMFrameBox2);
    G4LogicalVolume *logicGEMFrame = new G4LogicalVolume(solidGEMFrame, GEMFrameM, Form("GEM%dFrameLV", layerid));
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicGEMFrame, Form("GEM %d Frame", layerid), logicGEM, false, 0);

/*
    G4Box *GEMPipeBox = new G4Box(Form("GEM%dPipeBox", layerid), GEMCenterHalfXY - GEMFrameWidth / 2.0, GEMCenterHalfXY, GEMHalfT);
    G4SubtractionSolid *solidGEMPipe = new G4SubtractionSolid(Form("GEM%dPipeS", layerid), GEMPipeBox, GEMTube, 0, G4ThreeVector(-GEMFrameWidth / 2.0, 0, 0));
    G4LogicalVolume *logicGEMPipe = new G4LogicalVolume(solidGEMPipe, GEMFrameM, Form("GEM%dPipeLV", layerid));
    new G4PVPlacement(0, G4ThreeVector(-GEMCenterOffset + GEMFrameWidth / 2.0, 0, 0), logicGEMPipe, Form("GEM %d Pipe", layerid), logicGEM, false, 0);
*/

    // GEM Spacer
    G4double GEMSpacerOffset = 91.3 * mm;
    G4Box *GEMHSpacerBox1 = new G4Box(Form("GEM%dHSpacerBox1", layerid), GEMHalfX, GEMSpacerWh, GEMSpacerT / 2.0);
    G4Box *GEMHSpacerBox2 = new G4Box(Form("GEM%dHSpacerBox2", layerid), GEMHalfX - GEMCenterHalfXY + GEMFrameWidth / 2.0, GEMSpacerWh, GEMSpacerT / 2.0);
    G4Box *GEMVSpacerBox = new G4Box(Form("GEM%dVSpacerBox", layerid), GEMSpacerWv, GEMHalfY, GEMSpacerT / 2.0);
    G4UnionSolid *GEMSpacerPiece1 = new G4UnionSolid(Form("GEM%dSpacerPiece1", layerid), GEMVSpacerBox, GEMHSpacerBox1, 0, G4ThreeVector(GEMSpacerOffset, 204.0 * mm, 0));
    G4UnionSolid *GEMSpacerPiece2 = new G4UnionSolid(Form("GEM%dSpacerPiece2", layerid), GEMSpacerPiece1, GEMHSpacerBox1, 0, G4ThreeVector(GEMSpacerOffset, 409.3 * mm, 0));
    G4UnionSolid *GEMSpacerPiece3 = new G4UnionSolid(Form("GEM%dSpacerPiece3", layerid), GEMSpacerPiece2, GEMHSpacerBox2, 0, G4ThreeVector(GEMSpacerOffset + GEMCenterHalfXY - GEMFrameWidth / 2.0, 0, 0));
    G4UnionSolid *GEMSpacerPiece4 = new G4UnionSolid(Form("GEM%dSpacerPiece4", layerid), GEMSpacerPiece3, GEMHSpacerBox1, 0, G4ThreeVector(GEMSpacerOffset, -204.0 * mm, 0));
    G4UnionSolid *GEMSpacerPiece5 = new G4UnionSolid(Form("GEM%dSpacerPiece5", layerid), GEMSpacerPiece4, GEMHSpacerBox1, 0, G4ThreeVector(GEMSpacerOffset, -409.3 * mm, 0));
    G4UnionSolid *solidGEMSpacer = new G4UnionSolid(Form("GEM%dSpacerS", layerid), GEMSpacerPiece5, GEMVSpacerBox, 0, G4ThreeVector(GEMSpacerOffset * 2.0, 0, 0));
    G4LogicalVolume *logicGEMSpacer = new G4LogicalVolume(solidGEMSpacer, GEMFrameM, Form("GEM%dSpacerLV", layerid));

    // GEM Foil
    G4double GEMWinT = 25.0 * um;
    G4double GEMFoilT = 50.0 * um;
    G4double GEMCuT = 5.0 * um;
    G4double GEMGlueT = 60.0 * um;

    G4Box *solidGEMWin = new G4Box(Form("GEM%dWinBox", layerid), GEMHalfX, GEMHalfY, GEMWinT / 2.0);
//    G4SubtractionSolid *solidGEMWin = new G4SubtractionSolid(Form("GEM%dWinS", layerid), GEMWinBox, GEMSubBox, 0, G4ThreeVector(-GEMCenterOffset, 0, 0));
    G4LogicalVolume *logicGEMWin = new G4LogicalVolume(solidGEMWin, GEMFoilM, Form("GEM%dWinLV", layerid));

    G4Box *solidGEMFoil = new G4Box(Form("GEM%dFoilBox", layerid), GEMHalfX, GEMHalfY, GEMFoilT / 2.0);
//    G4SubtractionSolid *solidGEMFoil = new G4SubtractionSolid(Form("GEM%dFoilS", layerid), GEMFoilBox, GEMSubBox, 0, G4ThreeVector(-GEMCenterOffset, 0, 0));
    G4LogicalVolume *logicGEMFoil = new G4LogicalVolume(solidGEMFoil, GEMFoil0d8M, Form("GEM%dFoilLV", layerid));
    G4LogicalVolume *logicGEMFoil80 = new G4LogicalVolume(solidGEMFoil, GEMFoil0d2M, Form("GEM%dFoil80LV", layerid));
    G4LogicalVolume *logicGEMFoil350 = new G4LogicalVolume(solidGEMFoil, GEMFoilM, Form("GEM%dFoil350LV", layerid));
    G4LogicalVolume *logicGEMCathode = new G4LogicalVolume(solidGEMFoil, GEMFoilM, Form("GEM%dCathodeLV", layerid));

    G4Box *solidGEMCu = new G4Box(Form("GEM%dCuBox", layerid), GEMHalfX, GEMHalfY, GEMCuT / 2.0);
//    G4SubtractionSolid *solidGEMCu = new G4SubtractionSolid(Form("GEM%dCuS", layerid), GEMCuBox, GEMSubBox, 0, G4ThreeVector(-GEMCenterOffset, 0, 0));
    G4LogicalVolume *logicGEMCu = new G4LogicalVolume(solidGEMCu, GEMCu0d8M, Form("GEM%dCuLV", layerid));
    G4LogicalVolume *logicGEMCu80 = new G4LogicalVolume(solidGEMCu, GEMCu0d2M, Form("GEM%dCu80LV", layerid));
    G4LogicalVolume *logicGEMCu350 = new G4LogicalVolume(solidGEMCu, GEMCu0d75M, Form("GEM%dCu350LV", layerid));
    G4LogicalVolume *logicGEMCathodeCu = new G4LogicalVolume(solidGEMCu, GEMCuM, Form("GEM%dCathodeCuLV", layerid));

    G4Box *solidGEMGlue = new G4Box(Form("GEM%dGlueBox", layerid), GEMHalfX, GEMHalfY, GEMGlueT / 2.0);
//    G4SubtractionSolid *solidGEMGlue = new G4SubtractionSolid(Form("GEM%dGlueS", layerid), GEMGlueBox, GEMSubBox, 0, G4ThreeVector(-GEMCenterOffset, 0, 0));
    G4LogicalVolume *logicGEMGlue = new G4LogicalVolume(solidGEMGlue, GEMGlueM, Form("GEM%dGlueLV", layerid));

    G4double zoff = -GEMHalfT;

    new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMWinT / 2.0), logicGEMWin, Form("GEM %d Window", layerid), logicGEMGas, false, 0);
    zoff += GEMWinT;

    new G4PVPlacement(0, G4ThreeVector(-GEMSpacerOffset, 0, zoff + GEMSpacerT / 2.0), logicGEMSpacer, Form("GEM %d Spacer", layerid), logicGEMGas, false, 0);
    zoff += 3.0 * mm;

    new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMFoilT / 2.0), logicGEMCathode, Form("GEM %d Cathode", layerid), logicGEMGas, false, 0);
    zoff += GEMFoilT;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCathodeCu, Form("GEM %d Copper", layerid), logicGEMGas, false, 0);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(-GEMSpacerOffset, 0, zoff + GEMSpacerT / 2.0), logicGEMSpacer, Form("GEM %d Spacer", layerid), logicGEMGas, false, 1);
    zoff += 3.0 * mm;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCu, Form("GEM %d Copper", layerid), logicGEMGas, false, 1);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMFoilT / 2.0), logicGEMFoil, Form("GEM %d Foil", layerid), logicGEMGas, false, 0);
    zoff += GEMFoilT;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCu, Form("GEM %d Copper", layerid), logicGEMGas, false, 2);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(-GEMSpacerOffset, 0, zoff + GEMSpacerT / 2.0), logicGEMSpacer, Form("GEM %d Spacer", layerid), logicGEMGas, false, 2);
    zoff += 2.0 * mm;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCu, Form("GEM %d Copper", layerid), logicGEMGas, false, 3);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMFoilT / 2.0), logicGEMFoil, Form("GEM %d Foil", layerid), logicGEMGas, false, 1);
    zoff += GEMFoilT;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCu, Form("GEM %d Copper", layerid), logicGEMGas, false, 4);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(-GEMSpacerOffset, 0, zoff + GEMSpacerT / 2.0), logicGEMSpacer, Form("GEM %d Spacer", layerid), logicGEMGas, false, 3);
    zoff += 2.0 * mm;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCu, Form("GEM %d Copper", layerid), logicGEMGas, false, 5);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMFoilT / 2.0), logicGEMFoil, Form("GEM %d Foil", layerid), logicGEMGas, false, 2);
    zoff += GEMFoilT;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCu, Form("GEM %d Copper", layerid), logicGEMGas, false, 6);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(-GEMSpacerOffset, 0, zoff + GEMSpacerT / 2.0 + 0.01 * mm), logicGEMSpacer, Form("GEM %d Spacer", layerid), logicGEMGas, false, 4);
    zoff += 2.0 * mm;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCu80, Form("GEM %d Copper", layerid), logicGEMGas, false, 7);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMFoilT / 2.0), logicGEMFoil80, Form("GEM %d Foil", layerid), logicGEMGas, false, 3);
    zoff += GEMFoilT;

    if (!culess) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMCuT / 2.0), logicGEMCu350, Form("GEM %d Copper", layerid), logicGEMGas, false, 8);
        zoff += GEMCuT;
    }

    new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMFoilT / 2.0), logicGEMFoil350, Form("GEM %d Foil", layerid), logicGEMGas, false, 4);
    zoff += GEMFoilT;
    new G4PVPlacement(0, G4ThreeVector(0, 0, zoff + GEMGlueT / 2.0), logicGEMGlue, Form("GEM %d Glue", layerid), logicGEMGas, false, 0);
    zoff += GEMGlueT;

    new G4PVPlacement(0, G4ThreeVector(0, 0, GEMHalfT - GEMWinT / 2.0), logicGEMWin, Form("GEM %d Window", layerid), logicGEMGas, false, 1);


}



void g4rcDetectorConstruction::GetTargetIndex(G4String targ) {

	fTargIndex = -1;
	G4String targList[4] = {"H1","H2","H3","He3"};

	for(int i = 0; i<4; i++) {
		if(targ == targList[i]) {
			fTargIndex = i;
		}		
	}

	if (fTargIndex < 0.) {
		G4cout << "Bad target selected!" << G4endl;
		exit(1);
	}
}
    
    
void g4rcDetectorConstruction::SetTarget(G4String targ) {
	fTarg = targ;
	GetTargetIndex(fTarg);
}






