import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import { VRButton } from "three/addons/webxr/VRButton.js";
import * as Stats from "three/addons/libs/stats.module.js";
import { GUI } from "three/addons/libs/lil-gui.module.min.js";
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { GLTFExporter } from 'three/addons/exporters/GLTFExporter.js';

//////////////////////
/* GLOBAL VARIABLES */
//////////////////////
let stats;
let ambientLight;
var camera, scene, renderer;
var cameras = {};
var cameraHelpers = {};
var balloons = [];
var AxesHelpers = [];
var DroneWatchObject = null;
var grupoExportacao = new THREE.Group();
var DroneRotores = [];
var currentCameraIndex = 1;
var showCameraHelpers = false;
var wireframeMode = false;
var collisionHappening = false;
const activeKeys = new Set();
const clock = new THREE.Clock();
let delta = 0;
const sceneCenter = new THREE.Vector3(0, 0, 0);
const droneParams = {
	velocidadeX: 20,
	velocidadeY: 20,
	velocidadeZ: 20,
	velocidadeRotacao: Math.PI,
	velocidadeRotores: 3,
	currentYaw: 0,
	currentPitch: 0
};

/////////////////////
/* CREATE SCENE(S) */
/////////////////////
async function createScene() {
	scene = new THREE.Scene();
	scene.background = new THREE.Color(0x3B3B3B);

	const loader = new GLTFLoader();
	const gltf = await loader.loadAsync( 'pulseira.gltf' );
	gltf.scene.scale.set(0.2, 0.2, 0.2);
	gltf.scene.rotation.z = Math.PI;
	gltf.scene.position.set(0, -5, 0);
	gltf.scene.traverse((child) => {
		if (child.isMesh) {
			child.material = new THREE.MeshBasicMaterial({
				color: 0x71797E,
				wireframe: false
			});
		}
	});

	grupoExportacao.add(gltf.scene);
	
	DroneWatchObject = new DroneWatch(0,0,0);
	for (let i=0; i < DroneWatchObject.children.length; i++) {
		if(DroneWatchObject.children[i].constructor.name == "Rotor") {
			DroneRotores.push(DroneWatchObject.children[i]);
		}
	}

	grupoExportacao.add(DroneWatchObject);
	scene.add(grupoExportacao);
			
	createGroupOfBalloons(4);
	createAmbientLight();
}

/////////////
/* CAMERAS */
////////////

function createCameras() {
	const aspect = window.innerWidth / window.innerHeight;
	const orthoSize = 50;
	
	// Camera 1: Top view (orthogonal)
	cameras[1] = new THREE.OrthographicCamera(-orthoSize * aspect, orthoSize * aspect, orthoSize, -orthoSize, 1, 1000);
	cameras[1].position.set(0, 80, 0);
	cameras[1].lookAt(sceneCenter);
	
	// Camera 2: Side view (orthogonal)
	cameras[2] = new THREE.OrthographicCamera(-orthoSize * aspect, orthoSize * aspect, orthoSize, -orthoSize, 1, 1000);
	cameras[2].position.set(80, 0, 0);
	cameras[2].lookAt(sceneCenter);
	
	// Camera 3: Front view (orthogonal)
	cameras[3] = new THREE.OrthographicCamera(-orthoSize * aspect, orthoSize * aspect, orthoSize, -orthoSize, 1, 1000);
	cameras[3].position.set(0, 0, 80);
	cameras[3].lookAt(sceneCenter);
	
	// Camera 4: Fixed camera with orthogonal projection (outside main axes)
	cameras[4] = new THREE.OrthographicCamera(-orthoSize * aspect, orthoSize * aspect, orthoSize, -orthoSize, 1, 1000);
	cameras[4].position.set(60,60,60);
	cameras[4].lookAt(sceneCenter);
	
	// Camera 5: Fixed camera with perspective projection (outside main axes, same position as camera 4)
	cameras[5] = new THREE.PerspectiveCamera(70, aspect, 1, 1000);
	cameras[5].position.set(60,60,60);
	cameras[5].lookAt(sceneCenter);
	
	// Camera 6 is defined in droneWatch, as a child of it
	
	// camera helpers
	for (let i = 1; i <= 5; i++) {
		cameraHelpers[i] = new THREE.CameraHelper(cameras[i]);
		cameraHelpers[i].visible = showCameraHelpers;
		scene.add(cameraHelpers[i]);
	}
	
	// Set initial camera
	camera = cameras[currentCameraIndex];
}

function switchCamera(cameraIndex) {
	if (cameras[cameraIndex]) {
		currentCameraIndex = cameraIndex;
		camera = cameras[cameraIndex];
		console.log('Switched to camera ' + cameraIndex);
	}
}

/////////////////////
/* CREATE LIGHT(S) */
/////////////////////

function createAmbientLight() {
	ambientLight = new THREE.AmbientLight(0x404040, 0.5);
	scene.add(ambientLight);
}

/////////////
/* CLASSES*/
////////////
class DroneWatch extends THREE.Object3D {
	constructor(x, y, z){
		super()
		this.rotation.order = 'YXZ';
		this.deployed = false;
		this.deploying = false;
		this.isReturning = false;

		this.axesHelper = new THREE.AxesHelper(20);
		this.axesHelper.visible = showCameraHelpers;
		this.add(this.axesHelper);
		AxesHelpers.push(this.axesHelper);
		this.axesHelper.position.set(x, y, z);				// Center the axis on the drone watch
		
		this.position.set(x, y, z);
		this.buildWatch(x, y, z);

		this.bracos = [];
		this.buildBracos(x, y, z);

		this.antebracos = [];
		this.buildAntebracos(x, y, z);

		this.caixilharias = [];
		this.buildCaixilharias(x, y, z);

		this.rotores = [];
		this.buildRotores(x, y, z);
		this.buildWatchCamera(x, y, z);
	}

	returnToOrigin() {
		if (this.deployed || this.deploying) {
			this.isReturning = true;
			console.log('A iniciar retorno à origem...');
		}
	}

	buildWatch(x, y, z) {
		const geometryRelogio = new THREE.BoxGeometry(6, 2, 6);
		var materialRelogio = new THREE.MeshBasicMaterial( {color: 0x2E2E2E, wireframe: wireframeMode} );
	
		const relogio = new THREE.Mesh(geometryRelogio, materialRelogio);
		relogio.position.set(x, y, z);
			 
		this.add(relogio);
	
	}

	buildBracos(x, y, z) {
		var geometryBraco = new THREE.BoxGeometry(5, 1, 0.5);
		//var geometryBraco = new THREE.BoxGeometry(0, 1, 0.5);
		var materialBraco = new THREE.MeshBasicMaterial({
			color: 0x999B9B, 
			wireframe: wireframeMode
		});

		// Bracos positioned around the watch
		const braco1 = new THREE.Mesh(geometryBraco, materialBraco);
		//braco1.position.set(x - 4.5,  y + 0.25, z - 4.5);
		braco1.position.set(x, y + 0.25, z);
		braco1.rotation.y = - Math.PI / 4;
		this.bracos.push(braco1);
		this.add(braco1);

		const braco2 = new THREE.Mesh(geometryBraco, materialBraco);
		//braco2.position.set(x + 4.5, y + 0.25, z - 4.5);
		braco1.position.set(x, y + 0.25, z);
		braco2.rotation.y = - 3 * Math.PI / 4;
		this.bracos.push(braco2);
		this.add(braco2);

		const braco3 = new THREE.Mesh(geometryBraco, materialBraco);
		//braco3.position.set(x + 4.5,  y + 0.25, z + 4.5);
		braco1.position.set(x, y + 0.25, z);
		braco3.rotation.y = - Math.PI / 4;
		this.bracos.push(braco3);
		this.add(braco3);
		
		const braco4 = new THREE.Mesh(geometryBraco, materialBraco);
		//braco4.position.set(x - 4.5, y + 0.25, z + 4.5);
		braco1.position.set(x, y + 0.25, z);
		braco4.rotation.y = - 3 * Math.PI / 4;
		this.bracos.push(braco4);
		this.add(braco4);
	}

	buildAntebracos(x, y, z) {
		const geometryAntebraco = new THREE.BoxGeometry(2, 0.5, 0.25);
		var materialAntebraco = new THREE.MeshBasicMaterial({
			color: 0x999B9B, 
			wireframe: wireframeMode
		});

		const antebraco1 = new THREE.Mesh(geometryAntebraco, materialAntebraco);
		//antebraco1.position.set(x - 4.5 - 3*Math.sqrt(2)/2, y + 0.25, z - 4.5 - 3*Math.sqrt(2)/2);
		antebraco1.position.set(x, y + 0.25, z);
		antebraco1.rotation.y = - Math.PI / 4;
		this.antebracos.push(antebraco1);
		this.add(antebraco1);

		const antebraco2 = new THREE.Mesh(geometryAntebraco, materialAntebraco);
		//antebraco2.position.set(x + 4.5*2 - 3*Math.sqrt(2)/2, y + 0.25, z - 4.5 - 3*Math.sqrt(2)/2);
		antebraco1.position.set(x, y + 0.25, z);
		antebraco2.rotation.y = - 3 * Math.PI / 4;
		this.antebracos.push(antebraco2);
		this.add(antebraco2);

		const antebraco3 = new THREE.Mesh(geometryAntebraco, materialAntebraco);
		//antebraco3.position.set(x + 4.5*2 - 3*Math.sqrt(2)/2, y + 0.25, z + 4.5*2 - 3*Math.sqrt(2)/2);
		antebraco1.position.set(x, y + 0.25, z);
		antebraco3.rotation.y = - Math.PI / 4;
		this.antebracos.push(antebraco3);
		this.add(antebraco3);

		const antebraco4 = new THREE.Mesh(geometryAntebraco, materialAntebraco);
		//antebraco4.position.set(x - 4.5 - 3*Math.sqrt(2)/2, y + 0.25, z + 4.5*2 - 3*Math.sqrt(2)/2);
		antebraco1.position.set(x, y + 0.25, z);
		antebraco4.rotation.y = - 3 * Math.PI / 4;
		this.antebracos.push(antebraco4);
		this.add(antebraco4);
	}

	buildCaixilharias(x, y, z) {
		//const caixilharia1 = new Caixilharia(x - 7.5, y + 0.25, z - 7.5);
		const caixilharia1 = new Caixilharia(x, y + 0.25, z);
		this.caixilharias.push(caixilharia1);
		this.add(caixilharia1);

		//const caixilharia2 = new Caixilharia(x + 7.5, y + 0.25, z - 7.5);
		const caixilharia2 = new Caixilharia(x, y + 0.25, z);
		this.caixilharias.push(caixilharia2);
		this.add(caixilharia2);

		//const caixilharia3 = new Caixilharia(x + 7.5, y + 0.25, z + 7.5);
		const caixilharia3 = new Caixilharia(x, y + 0.25, z);
		this.caixilharias.push(caixilharia3);
		this.add(caixilharia3);

		//const caixilharia4 = new Caixilharia(x - 7.5, y + 0.25, z + 7.5);
		const caixilharia4 = new Caixilharia(x, y + 0.25, z);
		this.caixilharias.push(caixilharia4);
		this.add(caixilharia4);
	}

	buildRotores(x, y, z) {
		//const rotores1 = new Rotor(x - 7.5, y, z  - 7.5);
		const rotores1 = new Rotor(x, y, z);
		this.rotores.push(rotores1);
		this.add(rotores1)

		//const rotores2 = new Rotor(x + 7.5, y, z  - 7.5);
		const rotores2 = new Rotor(x, y, z);
		this.rotores.push(rotores2);
		this.add(rotores2)

		//const rotores3 = new Rotor(x + 7.5, y, z  + 7.5);
		const rotores3 = new Rotor(x, y, z);
		this.rotores.push(rotores3);
		this.add(rotores3)
		
		//const rotores4 = new Rotor(x - 7.5, y, z  + 7.5);
		const rotores4 = new Rotor(x, y, z);
		this.rotores.push(rotores4);
		this.add(rotores4)
	}

	buildWatchCamera(x, y, z) {
		const geometryCamera = new THREE.BoxGeometry(2, 0.5, 1);
		var materialCamera = new THREE.MeshBasicMaterial({
			color: 0x999B9B, 
			wireframe: wireframeMode
		});

		const watchCamera = new THREE.Mesh(geometryCamera, materialCamera);
		watchCamera.position.set(x, y + 1, z + 3);
		watchCamera.rotation.x = Math.PI / 4;
		this.add(watchCamera);

		// Camera 6: Mobile camera on smartwatch edge with perspective projection
		const aspect = window.innerWidth / window.innerHeight;
		cameras[6] = new THREE.PerspectiveCamera(70, aspect, 1, 1000);
		//cameras[6].position.set(x, y + 1, z + 3);
		cameras[6].position.set(0, 0.5, -3);
		//cameras[6].lookAt(x - 5, y + 1, z);
		cameras[6].lookAt(0, 0.5, -20);

		this.add(cameras[6]);
	

	}

	deploy() {
		this.deploying = true;
	}

	deployingAnimation(delta) {
		let finishedBracos = 0;
		let finishedAntebracos = 0;
		let finishedCaixilharias = 0;
		const speed = delta * 4;
		const targetB = this.deployed ? 0 : 4.5;
		const targetA = this.deployed ? 0 : 4.5 + 3 * Math.sqrt(2) / 2;
        const targetC = this.deployed ? 0 : 7.5;

		if (Math.abs(this.bracos[0].position.x - (-targetB)) > 0.1 || Math.abs(this.bracos[0].position.z - (-targetB)) > 0.1) {
			this.bracos[0].position.x += (this.bracos[0].position.x > -targetB ? -speed : speed);
			this.bracos[0].position.z += (this.bracos[0].position.z > -targetB ? -speed : speed);

			this.caixilharias[0].position.x += (this.caixilharias[0].position.x > -targetC ? -speed : speed);
            this.caixilharias[0].position.z += (this.caixilharias[0].position.z > -targetC ? -speed : speed);
            this.rotores[0].position.x += (this.rotores[0].position.x > -targetC ? -speed : speed);
            this.rotores[0].position.z += (this.rotores[0].position.z > -targetC ? -speed : speed);
		} else {
			this.bracos[0].position.set(-targetB, this.bracos[0].position.y, -targetB);
			finishedBracos++;
		}
		if (Math.abs(this.antebracos[0].position.x - (-targetA)) > 0.1 || Math.abs(this.antebracos[0].position.z - (-targetA)) > 0.1) {
			this.antebracos[0].position.x += (this.antebracos[0].position.x > -targetA ? -speed : speed);
			this.antebracos[0].position.z += (this.antebracos[0].position.z > -targetA ? -speed : speed);
		} else {
			this.antebracos[0].position.set(-targetA, this.antebracos[0].position.y, -targetA);
			finishedAntebracos++;
		}

		if (Math.abs(this.bracos[1].position.x - targetB) > 0.1 || Math.abs(this.bracos[1].position.z - (-targetB)) > 0.1) {
			this.bracos[1].position.x += (this.bracos[1].position.x < targetB ? speed : -speed);
			this.bracos[1].position.z += (this.bracos[1].position.z > -targetB ? -speed : speed);

			this.caixilharias[1].position.x += (this.caixilharias[1].position.x < targetC ? speed : -speed);
            this.caixilharias[1].position.z += (this.caixilharias[1].position.z > -targetC ? -speed : speed);
            this.rotores[1].position.x += (this.rotores[1].position.x < targetC ? speed : -speed);
            this.rotores[1].position.z += (this.rotores[1].position.z > -targetC ? -speed : speed);
		} else {
			this.bracos[1].position.set(targetB, this.bracos[1].position.y, -targetB);
			finishedBracos++;
		}
		if (Math.abs(this.antebracos[1].position.x - targetA) > 0.1 || Math.abs(this.antebracos[1].position.z - (-targetA)) > 0.1) {
			this.antebracos[1].position.x += (this.antebracos[1].position.x < targetA ? speed : -speed);
			this.antebracos[1].position.z += (this.antebracos[1].position.z > -targetA ? -speed : speed);
		} else {
			this.antebracos[1].position.set(targetA, this.antebracos[1].position.y, -targetA);
			finishedAntebracos++;
		}

		if (Math.abs(this.bracos[2].position.x - targetB) > 0.1 || Math.abs(this.bracos[2].position.z - targetB) > 0.1) {
			this.bracos[2].position.x += (this.bracos[2].position.x < targetB ? speed : -speed);
			this.bracos[2].position.z += (this.bracos[2].position.z < targetB ? speed : -speed);

			this.caixilharias[2].position.x += (this.caixilharias[2].position.x < targetC ? speed : -speed);
            this.caixilharias[2].position.z += (this.caixilharias[2].position.z < targetC ? speed : -speed);
            this.rotores[2].position.x += (this.rotores[2].position.x < targetC ? speed : -speed);
            this.rotores[2].position.z += (this.rotores[2].position.z < targetC ? speed : -speed);
		} else {
			this.bracos[2].position.set(targetB, this.bracos[2].position.y, targetB);
			finishedBracos++;
		}
		if (Math.abs(this.antebracos[2].position.x - targetA) > 0.1 || Math.abs(this.antebracos[2].position.z - targetA) > 0.1) {
			this.antebracos[2].position.x += (this.antebracos[2].position.x < targetA ? speed : -speed);
			this.antebracos[2].position.z += (this.antebracos[2].position.z < targetA ? speed : -speed);
		} else {
			this.antebracos[2].position.set(targetA, this.antebracos[2].position.y, targetA);
			finishedAntebracos++;
		}

		if (Math.abs(this.bracos[3].position.x - (-targetB)) > 0.1 || Math.abs(this.bracos[3].position.z - targetB) > 0.1) {
			this.bracos[3].position.x += (this.bracos[3].position.x > -targetB ? -speed : speed);
			this.bracos[3].position.z += (this.bracos[3].position.z < targetB ? speed : -speed);

			this.caixilharias[3].position.x += (this.caixilharias[3].position.x > -targetC ? -speed : speed);
        	this.caixilharias[3].position.z += (this.caixilharias[3].position.z < targetC ? speed : -speed);
            this.rotores[3].position.x += (this.rotores[3].position.x > -targetC ? -speed : speed);
            this.rotores[3].position.z += (this.rotores[3].position.z < targetC ? speed : -speed);
		} else {
			this.bracos[3].position.set(-targetB, this.bracos[3].position.y, targetB);
			finishedBracos++;
		}
		if (Math.abs(this.antebracos[3].position.x - (-targetA)) > 0.1 || Math.abs(this.antebracos[3].position.z - targetA) > 0.1) {
			this.antebracos[3].position.x += (this.antebracos[3].position.x > -targetA ? -speed : speed);
			this.antebracos[3].position.z += (this.antebracos[3].position.z < targetA ? speed : -speed);
		} else {
			this.antebracos[3].position.set(-targetA, this.antebracos[3].position.y, targetA);
			finishedAntebracos++;
		}


		if (finishedBracos === 4) {
			if (Math.abs(this.caixilharias[0].position.x - (-targetC)) > 0.1 || Math.abs(this.caixilharias[0].position.z - (-targetC)) > 0.1) {
				this.caixilharias[0].position.x += (this.caixilharias[0].position.x > -targetC ? -speed : speed);
				this.caixilharias[0].position.z += (this.caixilharias[0].position.z > -targetC ? -speed : speed);

				this.rotores[0].position.x += (this.rotores[0].position.x > -targetC ? -speed : speed);
				this.rotores[0].position.z += (this.rotores[0].position.z > -targetC ? -speed : speed);
			} else {
				this.caixilharias[0].position.set(-targetC, this.caixilharias[0].position.y, -targetC);
				this.rotores[0].position.set(-targetC, this.rotores[0].position.y, -targetC);
				finishedCaixilharias++;
			}

			if (Math.abs(this.caixilharias[1].position.x - targetC) > 0.1 || Math.abs(this.caixilharias[1].position.z - (-targetC)) > 0.1) {
				this.caixilharias[1].position.x += (this.caixilharias[1].position.x < targetC ? speed : -speed);
				this.caixilharias[1].position.z += (this.caixilharias[1].position.z > -targetC ? -speed : speed);

				this.rotores[1].position.x += (this.rotores[1].position.x < targetC ? speed : -speed);
				this.rotores[1].position.z += (this.rotores[1].position.z > -targetC ? -speed : speed);
			} else {
				this.caixilharias[1].position.set(targetC, this.caixilharias[1].position.y, -targetC);
				this.rotores[1].position.set(targetC, this.rotores[1].position.y, -targetC);
				finishedCaixilharias++;
			}

			if (Math.abs(this.caixilharias[2].position.x - targetC) > 0.1 || Math.abs(this.caixilharias[2].position.z - targetC) > 0.1) {
				this.caixilharias[2].position.x += (this.caixilharias[2].position.x < targetC ? speed : -speed);
				this.caixilharias[2].position.z += (this.caixilharias[2].position.z < targetC ? speed : -speed);

				this.rotores[2].position.x += (this.rotores[2].position.x < targetC ? speed : -speed);
				this.rotores[2].position.z += (this.rotores[2].position.z < targetC ? speed : -speed);
			} else {
				this.caixilharias[2].position.set(targetC, this.caixilharias[2].position.y, targetC);
				this.rotores[2].position.set(targetC, this.rotores[2].position.y, targetC);
				finishedCaixilharias++;
			}

			if (Math.abs(this.caixilharias[3].position.x - (-targetC)) > 0.1 || Math.abs(this.caixilharias[3].position.z - targetC) > 0.1) {
				this.caixilharias[3].position.x += (this.caixilharias[3].position.x > -targetC ? -speed : speed);
				this.caixilharias[3].position.z += (this.caixilharias[3].position.z < targetC ? speed : -speed);

				this.rotores[3].position.x += (this.rotores[3].position.x > -targetC ? -speed : speed);
				this.rotores[3].position.z += (this.rotores[3].position.z < targetC ? speed : -speed);
			} else {
				this.caixilharias[3].position.set(-targetC, this.caixilharias[3].position.y, targetC);
				this.rotores[3].position.set(-targetC, this.rotores[3].position.y, targetC);
				finishedCaixilharias++;
			}

		}

		if (finishedCaixilharias == 4) {
			this.deploying = false;
			this.deployed = !this.deployed;
			if (this.isReturning && !this.deployed) this.isReturning = false;
		}
	}



	toggleWireframe(){
		for (let i=0; i < this.children.length; i++) {
			if(this.children[i].constructor.name == "Caixilharia" || this.children[i].constructor.name == "Rotor") {
				this.children[i].toggleWireframe();
			} else {
				this.children[i].material.wireframe = wireframeMode;
			}
		}
	}
}
	
// When building a caixilharia provide the coordinates for the center of the box!!!
class Caixilharia extends THREE.Object3D {
	constructor(x, y, z) {
		super()

		this.buildParedes(x, y, z);
		

	}

	buildParedes(x, y, z) {
		const geometryParede1 = new THREE.BoxGeometry(4, 1, 0.5);
		const geometryParede2 = new THREE.BoxGeometry(0.5, 1, 4);

		var materialParede = new THREE.MeshBasicMaterial({
			color: 0x999B9B,
			wireframe: wireframeMode
		});

		// parede1 and parede2 use geoemtryParede1 so they grow along the X axis therefore their position must be changed along the Z axis
		const parede1 = new THREE.Mesh(geometryParede1, materialParede);
		parede1.position.set(x,  y,  z - 1.5);
		this.add(parede1);
		
		
		const parede2 = new THREE.Mesh(geometryParede1, materialParede);
		parede2.position.set(x, y,  z + 1.5);
		this.add(parede2);

		const parede3 = new THREE.Mesh(geometryParede2, materialParede);
		parede3.position.set(x - 1.5, y,  z);
		this.add(parede3);

		const parede4 = new THREE.Mesh(geometryParede2, materialParede);
		parede4.position.set(x + 1.5, y,  z);
		this.add(parede4);

	}

	toggleWireframe(){
		for (let i=0; i < this.children.length; i++) {
			this.children[i].material.wireframe = wireframeMode;
		}
	}
}

// When building rotor provide the coordinates for the center 
class Rotor extends THREE.Object3D {
	constructor(x, y, z) {
		super()

		this.position.set(x, y, z);				// Set the position for the whole object, so that the helices can be fixed on the local center
		this.buildHelices();
		this.buildCollisionShape();
	}

	buildHelices() {
		const geometryHelice = new THREE.BoxGeometry(2.5, 0.25, 0.5);

		var materialHelice = new THREE.MeshBasicMaterial({
			color: 0x999B9B,
			//color: 0x2E2E2E,
			wireframe: wireframeMode
		});

		const helice1 = new THREE.Mesh(geometryHelice, materialHelice);
		helice1.position.set(0,  0,  0);		// set it to local origin so it spins around itself
		helice1.rotation.y = - Math.PI / 4;
		this.add(helice1);
		
		const helice2 = new THREE.Mesh(geometryHelice, materialHelice);
		helice2.position.set(0, 0, 0);
		helice2.rotation.y = Math.PI / 4;
		this.add(helice2);
	}
	
	buildCollisionShape() {
		var geometry = new THREE.SphereGeometry(3,16,11);
		var material = new THREE.MeshBasicMaterial({
			color: 0x00ff00, 
			wireframe: true
		});
		var mesh = new THREE.Mesh(geometry,material);
		mesh.position.set(0, 0, 0);
		mesh.visible = false;
		this.add(mesh);
	}

	toggleWireframe(){
		for (let i=0; i < this.children.length; i++) {
			this.children[i].material.wireframe = wireframeMode;
		}
	}
}

class Balloon extends THREE.Object3D {
	constructor(x, y, z) {
		super()
		
		this.axesHelper = new THREE.AxesHelper(20);
		this.axesHelper.visible = showCameraHelpers;
		this.add(this.axesHelper);
		AxesHelpers.push(this.axesHelper);
		this.axesHelper.position.set(x, y, z);				// Center the axis on the balloons
		
		this.position.set(x, y, z);
		this.buildPlastic();
		this.buildKnot();
		this.buildString();
		this.buildCollisionShape();
	}

	buildPlastic() {
		var geometry = new THREE.SphereGeometry(5,16,11);
		var material = new THREE.MeshBasicMaterial({color: 0xff0000, wireframe: wireframeMode});
		var mesh = new THREE.Mesh(geometry,material);
		mesh.position.set(0,0,0);
		geometry.scale(1,1.1,1);
		this.add(mesh);
	}
	
	buildKnot() {
		var geometry = new THREE.ConeGeometry(1.25,2.5,5);
		var material = new THREE.MeshBasicMaterial({
			color: 0xff0000, 
			wireframe: wireframeMode 
		});
		var mesh = new THREE.Mesh(geometry,material);
		mesh.position.set(0, -5, 0);
		this.add(mesh);
	}

	buildString() {
		var geometry = new THREE.CylinderGeometry(0.25,0.25,20,8);
		var material = new THREE.MeshBasicMaterial({color: 0xB2BEB5})
		var mesh = new THREE.Mesh(geometry, material);
		mesh.position.set(0, 0 - 15, 0);
		this.add(mesh);
	}

	buildCollisionShape() {
		var geometry = new THREE.SphereGeometry(7,16,11);
		var material = new THREE.MeshBasicMaterial({color: 0x00ff00, wireframe: true});
		var mesh = new THREE.Mesh(geometry,material);
		mesh.position.set(this.position.x, this.position.y, this.position.z);
		mesh.visible = false;
		this.add(mesh);
	}

	toggleWireframe(){
		for (let i=0; i < this.children.length; i++) {
			this.children[i].material.wireframe = wireframeMode;
		}
	}
}

function createGroupOfBalloons(numBalloons) {
    for (let i = 0; i < numBalloons; i++) {
		if (Math.random() > 0.5) {
			createBalloon(Math.random()*15 + 30,Math.random()*20+10,Math.random()*15 +30);
		}
		else {
			createBalloon(Math.random()*(-15) - 30,Math.random()*20+10,Math.random()*(30));
		}
    }
	
    console.log('Created ' + numBalloons + ' balloons');
}

function createBalloon(x, y, z) {
	console.log(x, y, z);
	const balloon = new Balloon(x, y, z);
	balloons.push(balloon);
	scene.add(balloon);
}

//////////////////////
/* CHECK COLLISIONS */
//////////////////////
function checkCollisions() {
	// Para cada rotor
    for (let i = 0; i < 4; i++) {
        const rotor = DroneRotores[i];
		// Para cada balão
        for (let j = 0; j < balloons.length; j++) {
            const balloon = balloons[j];
            // balloon collision radius = 7, rotor collision sphere radius = 3 -> collision if distance < 10
            const distance = rotor.getWorldPosition(new THREE.Vector3()).distanceTo(balloon.getWorldPosition(new THREE.Vector3()));
            if (distance <= 10) {
                console.log('COLLIDED!');
                handleCollisions(rotor, balloon);
            }
        }
    }
}

///////////////////////
/* HANDLE COLLISIONS */
///////////////////////
function handleCollisions(rotor, balloon) {
    console.log('Collision detected!');
	collisionHappening = true;
	animateBalloonDisappear(balloon);
}

////////////
/* UPDATE */
////////////
function update() {
	//requestAnimationFrame(animate);
	requestAnimationFrame(update);
	delta += clock.getDelta();
	const interval = 1 / 60;		// 60 fps
	checkCollisions();

	// Limit fps for performance
	if (delta > interval) {
		// Only rotate Rotores quando o drone está deployed e quando não está deploying
        // (a 2a condicao seria suficiente exceto que assim eles ficariam a rodar dentro do relogio quando estava tudo recolhido)
        if (DroneWatchObject.deployed && !DroneWatchObject.deploying) {
            for (let i = 0; i < DroneRotores.length; i++) {
                DroneRotores[i].rotation.y += delta * droneParams.velocidadeRotores;
            }
        }

		if (DroneWatchObject && DroneWatchObject.deploying) {
			DroneWatchObject.deployingAnimation(delta);
		}

		if (DroneWatchObject && DroneWatchObject.isReturning) {
			// Define a velocidade constante de retorno
			const velocidadeRetornoPos = 15;
			const velocidadeRetornoRot = 2;

			// 1. Translacao Constante
			const targetPos = new THREE.Vector3(0, 0, 0);
			const distancePos = DroneWatchObject.position.distanceTo(targetPos);
			const stepPos = velocidadeRetornoPos * delta;

			// Se a distancia for maior que o passo, move-se o passo exato usando lerp com a fracao correspondente
			if (distancePos > stepPos) {
				DroneWatchObject.position.lerp(targetPos, stepPos / distancePos);
			} else {
				DroneWatchObject.position.copy(targetPos);
			}

			// 2. Rotacao Constante
			const targetRot = new THREE.Quaternion();
			// rotateTowards move o quaternian em direcao ao alvo a uma velocidade angular constante
			DroneWatchObject.quaternion.rotateTowards(targetRot, velocidadeRetornoRot * delta);

			// 3. Verificacao de chegada (ambos perto de 0)
			if (DroneWatchObject.position.length() < 0.1 && DroneWatchObject.quaternion.angleTo(targetRot) < 0.05 && !DroneWatchObject.deploying && DroneWatchObject.deployed) {
				DroneWatchObject.position.set(0, 0, 0);
				DroneWatchObject.rotation.set(0, 0, 0);
				droneParams.currentYaw = 0;
				droneParams.currentPitch = 0;
				DroneWatchObject.deploy();
			}
		} else if (DroneWatchObject && DroneWatchObject.deployed && !collisionHappening) {
			let dx = 0;
			let dy = 0;
			let dz = 0;

			// Calcular a intencao de movimento
			if (activeKeys.has('i') || activeKeys.has('I')) droneParams.currentYaw += droneParams.velocidadeRotacao * delta;
			if (activeKeys.has('k') || activeKeys.has('K')) droneParams.currentYaw -= droneParams.velocidadeRotacao * delta;
			if (activeKeys.has('o') || activeKeys.has('O')) droneParams.currentPitch -= droneParams.velocidadeRotacao * delta;
			if (activeKeys.has('l') || activeKeys.has('L')) droneParams.currentPitch += droneParams.velocidadeRotacao * delta;

			// 1. Limitar o Pitch
			const limitePitch = Math.PI / 4;
			droneParams.currentPitch = Math.max(-limitePitch, Math.min(limitePitch, droneParams.currentPitch));

			// 2. Aplicar a rotacao de forma absoluta usando a ordem YXZ
			DroneWatchObject.rotation.set(droneParams.currentPitch, droneParams.currentYaw, 0, 'YXZ');

			// As translacoes em X, Y e Z continuam as mesmas
			if (activeKeys.has('a') || activeKeys.has('A')) dx -= droneParams.velocidadeX * delta;
			if (activeKeys.has('d') || activeKeys.has('D')) dx += droneParams.velocidadeX * delta;
			if (activeKeys.has('w') || activeKeys.has('W')) dy += droneParams.velocidadeY * delta;
			if (activeKeys.has('s') || activeKeys.has('S')) dy -= droneParams.velocidadeY * delta;
			if (activeKeys.has('u') || activeKeys.has('U')) dz -= droneParams.velocidadeZ * delta;
			if (activeKeys.has('j') || activeKeys.has('J')) dz += droneParams.velocidadeZ * delta;

			if (dx !== 0) DroneWatchObject.translateX(dx);
			if (dy !== 0) DroneWatchObject.translateY(dy);
			if (dz !== 0) DroneWatchObject.translateZ(dz);
		}

		render();
		delta = delta % interval;
	}
}

/////////////
/* DISPLAY */
/////////////
function render() {
	renderer.render(scene, camera);
}

////////////////////////
/* HUD & GUI CONTROLS */
////////////////////////
function createHUD() {
	const hud = document.createElement('div');
	hud.id = 'hud';
	hud.style.position = 'absolute';
	hud.style.bottom = '2vh';
	hud.style.right = '2vw';
	hud.style.color = 'white';
	hud.style.fontFamily = 'monospace';
	hud.style.backgroundColor = 'rgba(0,0,0,0.7)';
	hud.style.padding = '1.5vmin';
	hud.style.fontSize = '1.2vmin';
	hud.style.borderRadius = '0.5vmin';
	hud.style.pointerEvents = 'none';
	hud.style.userSelect = 'none';

	hud.innerHTML = `
		<h3 style="margin-top:0; border-bottom: 0.1vmin solid white; padding-bottom: 0.5vmin; font-size: 1.5vmin;">Teclas Ativas</h3>
		<div id="key-123456">1-6: Alternar Câmaras</div>
		<div id="key-7">7: Modo Wireframe</div>
		<div id="key-h">H: Mostrar Helpers</div>
		<br>
		<div id="key-q">Q: Descolar/Regressar e Aterrar</div>
		<div id="key-ws">W/S: Translação Y</div>
		<div id="key-ad">A/D: Translação X</div>
		<div id="key-uj">U/J: Translação Z</div>
		<div id="key-ik">I/K: Rotação Yaw</div>
		<div id="key-ol">O/L: Rotação Pitch</div>
	`;
	document.body.appendChild(hud);
}

function updateHUD() {
	const checkKey = (keysStr, elementId) => {
		const el = document.getElementById(elementId);
		if (!el) return;
		const isActive = keysStr.split('').some((key) => activeKeys.has(key.toLowerCase()) || activeKeys.has(key.toUpperCase()));
		el.style.color = isActive ? '#00ff00' : 'white';
		el.style.fontWeight = isActive ? 'bold' : 'normal';
	};

	checkKey('123456', 'key-123456');
	checkKey('7', 'key-7');
	checkKey('h', 'key-h');
	checkKey('q', 'key-q');
	checkKey('ws', 'key-ws');
	checkKey('ad', 'key-ad');
	checkKey('uj', 'key-uj');
	checkKey('ik', 'key-ik');
	checkKey('ol', 'key-ol');
}

function createGUI() {
	const gui = new GUI({ title: 'Parâmetros da Cena' });

	const sceneParams = {
		intensidadeLuz: 0.5,
		corFundo: '#3B3B3B'
	};

	// const sceneFolder = gui.addFolder('Cena');
	// sceneFolder.add(sceneParams, 'intensidadeLuz', 0, 2).name('Luz Ambiente').onChange((value) => {
	// 	if (ambientLight) ambientLight.intensity = value;
	// });

	// sceneFolder.addColor(sceneParams, 'corFundo').name('Cor do Fundo').onChange((value) => {
	// 	scene.background.set(value);
	// });

	// const droneFolder = gui.addFolder('Controlos do Drone');
	// droneFolder.add(droneParams, 'velocidadeX', 0, 100).name('Velocidade Eixo X');
	// droneFolder.add(droneParams, 'velocidadeY', 0, 100).name('Velocidade Eixo Y');
	// droneFolder.add(droneParams, 'velocidadeZ', 0, 100).name('Velocidade Eixo Z');
	// droneFolder.add(droneParams, 'velocidadeRotacao', 0, Math.PI * 4).name('Velocidade Rotação');
	// droneFolder.add(droneParams, 'velocidadeRotores', 0, 50).name('Velocidade Rotores');
	gui.add(droneParams, 'velocidadeX', 0, 100).name('Velocidade Eixo X');
	gui.add(droneParams, 'velocidadeY', 0, 100).name('Velocidade Eixo Y');
	gui.add(droneParams, 'velocidadeZ', 0, 100).name('Velocidade Eixo Z');
	gui.add(droneParams, 'velocidadeRotacao', 0, Math.PI * 4).name('Velocidade Rotação');
	gui.add(droneParams, 'velocidadeRotores', 0, 50).name('Velocidade Rotores');
}

function updateUIScaling() {
	const minDimension = Math.min(window.innerWidth, window.innerHeight);
	const statsScale = Math.max(0.85, Math.min(1.35, minDimension / 900));
	document.documentElement.style.setProperty('--stats-scale', statsScale.toFixed(2));
}

////////////////////////////////
/* INITIALIZE ANIMATION CYCLE */
////////////////////////////////
async function init() {
	renderer = new THREE.WebGLRenderer();
	renderer.setSize(window.innerWidth, window.innerHeight);

	// This adds the renderer to the HTML document
	document.body.appendChild(renderer.domElement);

	stats = new (Stats.default || Stats.Stats)();
	stats.showPanel(0);
	document.body.appendChild(stats.dom);

	await createScene();
	createCameras();
	createHUD();
	createGUI();
	updateUIScaling();

	window.addEventListener('keydown', onKeyDown);
	window.addEventListener('keyup', onKeyUp);
	window.addEventListener('resize', onResize);

	renderer.setAnimationLoop(animate);
} 

/////////////////////
/* ANIMATION CYCLE */
/////////////////////
function animate() {
	if (stats) stats.begin();

	update();

	if (stats) stats.end();
}

function animateBalloonDisappear(balloon) {
    const DURATION = 500;
    const startTime = performance.now();

    function step(now) {
        const elapsed = now - startTime;
        const t = Math.min(elapsed / DURATION, 1);

        const s = 1 - t;
        balloon.scale.set(s, s, s);

        balloon.traverse(obj => {
            if (obj.isMesh && obj.material) {
                obj.material.opacity = 1 - t;
            }
        });

        if (t < 1) {
            requestAnimationFrame(step);
        } else {
            scene.remove(balloon);
            const index = balloons.indexOf(balloon);
            if (index !== -1) balloons.splice(index, 1);
			collisionHappening = false;
        }
    }

    requestAnimationFrame(step);
}

////////////////////////////
/* RESIZE WINDOW CALLBACK */
////////////////////////////
function onResize() {
	if (!renderer) return;

	renderer.setSize(window.innerWidth, window.innerHeight);

	const aspect = window.innerWidth / window.innerHeight;
	for (let i = 1; i <= 6; i++) {
		const camera = cameras[i];
		if (!camera) continue;

		if (camera.isOrthographicCamera) {
			camera.left = -50 * aspect;
			camera.right = 50 * aspect;
			camera.top = 50;
			camera.bottom = -50;
		} else {
			camera.aspect = aspect;
		}

		camera.updateProjectionMatrix();
	}

	updateUIScaling();
}

///////////////////////////
/* GLTF EXPORT FUNCTIONS */
///////////////////////////
function exportarParaGLTF(objetoParaExportar) {
	if (!objetoParaExportar) {
		console.warn('Nada para exportar: objeto alvo inexistente.');
		return;
	}

	const exporter = new GLTFExporter();
	const options = { binary: true };

	exporter.parse(
		objetoParaExportar,
		function (result) {
			if (result instanceof ArrayBuffer) {
				guardarFicheiro(result, 'dronewatch_completo.glb');
			} else {
				const output = JSON.stringify(result, null, 2);
				guardarString(output, 'dronewatch_completo.gltf');
			}
			console.log('Exportacao concluida!');
		},
		function (error) {
			console.error('Um erro ocorreu durante a exportacao:', error);
		},
		options
	);
}

function guardarFicheiro(buffer, filename) {
	const blob = new Blob([buffer], { type: 'application/octet-stream' });
	const link = document.createElement('a');
	link.style.display = 'none';
	link.href = URL.createObjectURL(blob);
	link.download = filename;
	document.body.appendChild(link);
	link.click();
	document.body.removeChild(link);
}

function guardarString(text, filename) {
	const blob = new Blob([text], { type: 'text/plain' });
	const link = document.createElement('a');
	link.style.display = 'none';
	link.href = URL.createObjectURL(blob);
	link.download = filename;
	document.body.appendChild(link);
	link.click();
	document.body.removeChild(link);
}

///////////////////////
/* KEY DOWN CALLBACK */
///////////////////////
function onKeyDown(e) {
	activeKeys.add(e.key);
	updateHUD();

	const key = e.key.toLowerCase();
	if (!collisionHappening){
		if (key === '1') {
			switchCamera(1);
		} else if (key === '2') {
			switchCamera(2);
		} else if (key === '3') {
			switchCamera(3);
		} else if (key === '4') {
			switchCamera(4);
		} else if (key === '5') {
			switchCamera(5);
		} else if (key === '6') {
			switchCamera(6);
		} else if (key === '7') {
			toggleWireframeMode();
		} else if (key === 'h' || key === 'H') {
			toggleHelpers();
		} else if (key === 'e') {
			exportarParaGLTF(grupoExportacao);
		}
		if (key === 'q') {
			// Verifica se a posição do drone está fora da origem
			if (DroneWatchObject.position.length() > 0.1) {
				DroneWatchObject.returnToOrigin();
			} else {
				// Se já estiver na origem, faz o comportamento normal (descolar/aterrar)
				DroneWatchObject.deploy();
			}
		}
	}
	
}

///////////////////////
/* KEY UP CALLBACK */
///////////////////////
function onKeyUp(e) {
	activeKeys.delete(e.key);
	updateHUD();
}

function toggleHelpers() {
	showCameraHelpers = !showCameraHelpers;
	for (let i = 1; i <= 6; i++) {
		cameraHelpers[i].visible = showCameraHelpers;
	}
	// Toggle balloon axes helpers
	for (let i = 0; i < AxesHelpers.length; i++) {
		AxesHelpers[i].visible = showCameraHelpers;
	}
	console.log('Axes helpers ' + (showCameraHelpers ? 'visible' : 'hidden'));
}

function toggleWireframeMode() {
	wireframeMode = !wireframeMode;

	for (let i = 0; i < balloons.length; i++) {
		balloons[i].toggleWireframe();
	}
	DroneWatchObject.toggleWireframe();

	console.log('Changed to ' + (wireframeMode ? 'wireframe only mode' : 'solid mode'));


}
init();
//animate();

//checkCollisions();
