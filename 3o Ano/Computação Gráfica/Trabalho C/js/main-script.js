import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import { VRButton } from "three/addons/webxr/VRButton.js";
import * as Stats from "three/addons/libs/stats.module.js";
import { GUI } from "three/addons/libs/lil-gui.module.min.js";
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { AnaglyphEffect } from 'three/addons/effects/AnaglyphEffect.js';

//////////////////////
/* GLOBAL VARIABLES */
//////////////////////
let scene, camera, renderer, gui, anaglyphEffect;
let activeModel;

// Ter modelos como variaveis globais para poder fazer o switchModel()
let tesseract, animal, artemis;
const clock = new THREE.Clock(); // para animações do tesseract

// Luzes globais
let ambientLight;
let directionalLight;
let allPointLights = [];
let allSpotLights = [];
let tesseractLights = { points: [], spots: [] };
let animalLights    = { points: [], spots: [] };
let artemisLights   = { points: [], spots: [] };

// Estado partilhado do HUD para controlar a cena
const hudSettings = {
    modeloAtual: 'Nenhum',
    luzDireccional: true,
    luzPontuais: false,
    spotlights: false,
    material: 'Lambert (Gouraud)',
    iluminacaoAtiva: true,
    efeitoAnaglifico: false
};

/////////////////////
/* CREATE SCENE(S) */
/////////////////////
async function createScene() {
    scene = new THREE.Scene();
    // Definir o fundo da cena com a cor preta.
    scene.background = new THREE.Color(0x000000);
	
    tesseract = new Tesseract();
    // Não adicionar à cena aqui — só é adicionado via switchActiveModel

    const loader = new GLTFLoader();
    const gltf = await loader.loadAsync( 'scene.gltf' );
    const animalModel = gltf.scene;

    animalModel.traverse((child) => {
        if (child.isMesh) {
            child.material = new THREE.MeshBasicMaterial({
                color: 0xffffff,
                wireframe: false
            });
        }
    });

    const animalBox = new THREE.Box3().setFromObject(animalModel);
    const animalCenter = animalBox.getCenter(new THREE.Vector3());
    animalModel.position.sub(animalCenter);

    animal = new THREE.Group();
    animal.add(animalModel);
    
    // Não adicionar à cena aqui — só é adicionado via switchActiveModel

    const artemisGltf = await loader.loadAsync('artemis/slsv2.gltf');
    const artemisModel = artemisGltf.scene;

    const box = new THREE.Box3().setFromObject(artemisModel);
    const center = box.getCenter(new THREE.Vector3());
    const size = box.getSize(new THREE.Vector3());
    const maxDim = Math.max(size.x, size.y, size.z);
    const scale = 8 / maxDim;
    artemisModel.scale.setScalar(scale);
    artemisModel.rotation.y = Math.PI / 2;
    artemisModel.position.set(
        -center.x * scale,
        -center.y * scale,
        -center.z * scale
    );

    // Envolver num Group com scale=1 para que as luzes ancoradas fiquem
    // em coordenadas world-space correctas (não afectadas pela escala do modelo)
    artemis = new THREE.Group();
    artemis.add(artemisModel);

    createLights();
}

//////////////////////
/* CREATE CAMERA(S) */
//////////////////////
function createCamera() {
    camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
    camera.position.set(0, 0, 10);
    camera.lookAt(0, 0, 0);
}

/////////////////////
/* CREATE LIGHT(S) */
/////////////////////

function createModelLights(model, lightsObj) {
    // Luz pontual 1 — tom quente, à frente e acima do modelo
    const point1 = new THREE.PointLight(0xff9944, 15.0, 15);
    point1.position.set(3, 4, 3);
    point1.visible = hudSettings.luzPontuais;
    model.add(point1);
    lightsObj.points.push(point1);
    allPointLights.push(point1);

    // Luz pontual 2 — tom frio, atrás e abaixo do modelo
    const point2 = new THREE.PointLight(0x4499ff, 15.0, 15);
    point2.position.set(-3, -4, -3);
    point2.visible = hudSettings.luzPontuais;
    model.add(point2);
    lightsObj.points.push(point2);
    allPointLights.push(point2);

    // Spotlight 1 — de baixo para cima, com componente Z para iluminar faces frontais
    const spot1 = new THREE.SpotLight(0xffffff, 20.0, 20, Math.PI / 8, 0.3);
    spot1.position.set(-7, 5, 5);
    spot1.visible = hudSettings.spotlights;
    model.add(spot1);
    model.add(spot1.target);
    lightsObj.spots.push(spot1);
    allSpotLights.push(spot1);

    // Spotlight 2 — do lado direito para o centro, com componente Z para iluminar faces frontais
    const spot2 = new THREE.SpotLight(0xffffff, 20.0, 20, Math.PI / 8, 0.3);
    spot2.position.set(7, -4, 5);
    spot2.visible = hudSettings.spotlights;
    model.add(spot2);
    model.add(spot2.target);
    lightsObj.spots.push(spot2);
    allSpotLights.push(spot2);
}

function createLights() {
    // Luz ambiente: baixa intensidade, tonalidade esbranquiçada
    ambientLight = new THREE.AmbientLight(0xffffff, 0.3);
    scene.add(ambientLight);

    // Luz direcional
    directionalLight = new THREE.DirectionalLight(0xffffff, 1.2);
    directionalLight.position.set(5, 8, 5);
    directionalLight.visible = hudSettings.luzDireccional;
    scene.add(directionalLight);

    // Luzes ancoradas a cada modelo
    createModelLights(tesseract, tesseractLights);
    createModelLights(animal, animalLights);
    createModelLights(artemis, artemisLights);
}

////////////////////////
/* CREATE OBJECT3D(S) */
////////////////////////

class Tesseract extends THREE.Object3D {
    constructor() {
        super();

        // Cubo exterior: ±2.5 nos três eixos (cubo 5×5×5)
        // Cubo interior: ±0.5 nos três eixos (cubo 1×1×1)
        const v = {
            A: [-2.5,  2.5,  2.5],  // outer front face
            B: [ 2.5,  2.5,  2.5],
            C: [-2.5, -2.5,  2.5],
            D: [ 2.5, -2.5,  2.5],
            E: [-0.5,  0.5, -0.5],  // inner back face
            F: [ 0.5,  0.5, -0.5],
            G: [-0.5, -0.5, -0.5],
            H: [ 0.5, -0.5, -0.5],
            I: [-2.5,  2.5, -2.5],  // outer back face
            J: [ 2.5,  2.5, -2.5],
            K: [-2.5, -2.5, -2.5],
            L: [ 2.5, -2.5, -2.5],
            M: [-0.5,  0.5,  0.5],  // inner front face
            N: [ 0.5,  0.5,  0.5],
            O: [-0.5, -0.5,  0.5],
            P: [ 0.5, -0.5,  0.5],
        };

        // 32 edges of the tesseract
        const edges = [
            // Outer cube – front face (z = +0.5)
            ['A','B'], ['B','D'], ['D','C'], ['C','A'],
            // Outer cube – back face (z = -0.5)
            ['I','J'], ['J','L'], ['L','K'], ['K','I'],
            // Outer cube – depth edges
            ['A','I'], ['B','J'], ['C','K'], ['D','L'],
            // Inner cube – front face (z = +0.5)
            ['M','N'], ['N','P'], ['P','O'], ['O','M'],
            // Inner cube – back face (z = -0.5)
            ['E','F'], ['F','H'], ['H','G'], ['G','E'],
            // Inner cube – depth edges
            ['M','E'], ['N','F'], ['O','G'], ['P','H'],
            // 4th-dimension connecting edges (outer front ↔ inner front)
            ['A','M'], ['B','N'], ['C','O'], ['D','P'],
            // 4th-dimension connecting edges (outer back ↔ inner back)
            ['I','E'], ['J','F'], ['K','G'], ['L','H'],
        ];

        const positions = [];
        for (const [a, b] of edges) {
            positions.push(...v[a], ...v[b]);
        }

        const edgeGeo = new THREE.BufferGeometry();
        edgeGeo.setAttribute('position', new THREE.BufferAttribute(new Float32Array(positions), 3));
        const edgeMat = new THREE.LineBasicMaterial({ color: 0xffffff });
        this.add(new THREE.LineSegments(edgeGeo, edgeMat));

        // ── Semi-transparent faces ──────────────────────────────────────────
        // Each quad [a,b,c,d] split into 2 triangles: (a,b,c) and (a,c,d).
        // 24 faces: 6 outer cube + 6 inner cube + 12 connecting tunnel bands.
        const quads = [
            // Outer cube (6)
            ['A','B','D','C'],   // front  (z=+0.5)
            ['J','I','K','L'],   // back   (z=-0.5)
            ['I','J','B','A'],   // top    (y=+2.5)
            ['C','D','L','K'],   // bottom (y=-2.5)
            ['I','A','C','K'],   // left   (x=-2.5)
            ['B','J','L','D'],   // right  (x=+2.5)


            // Connecting tunnel faces (12)
            // Front annular frame (z=+0.5)
            ['A','B','N','M'],   // top band
            ['D','C','O','P'],   // bottom band
            ['A','C','O','M'],   // left band
            ['B','D','P','N'],   // right band
            // Back annular frame (z=-0.5)
            ['J','I','E','F'],   // top band
            ['K','L','H','G'],   // bottom band
            ['I','K','G','E'],   // left band
            ['L','J','F','H'],   // right band
            // Top frustum lateral sides
            ['I','A','M','E'],   // left  (x: -2.5 -> -0.5)
            ['B','J','F','N'],   // right (x: +2.5 -> +0.5)
            // Bottom frustum lateral sides
            ['C','K','G','O'],   // left
            ['L','D','P','H'],   // right
        ];

        const facePositions = [];
        const faceUVs = [];

        const uvA = [0, 1], uvB = [1, 1], uvC = [1, 0], uvD = [0, 0];
        for (const [a, b, c, d] of quads) {
            facePositions.push(...v[a], ...v[b], ...v[c]); // tri 1
            facePositions.push(...v[a], ...v[c], ...v[d]); // tri 2
            faceUVs.push(...uvA, ...uvB, ...uvC); // tri 1
            faceUVs.push(...uvA, ...uvC, ...uvD); // tri 2
        }

        const faceGeo = new THREE.BufferGeometry();
        faceGeo.setAttribute('position', new THREE.BufferAttribute(new Float32Array(facePositions), 3));
        faceGeo.setAttribute('uv', new THREE.BufferAttribute(new Float32Array(faceUVs), 2));
        faceGeo.computeVertexNormals();

        const normalMapTexture = new THREE.TextureLoader().load('NormalMap.png');
        normalMapTexture.wrapS = THREE.RepeatWrapping;
        normalMapTexture.wrapT = THREE.RepeatWrapping;
        normalMapTexture.repeat.set(1, 1);

        const faceMat = new THREE.MeshPhongMaterial({
            transparent: true,
            opacity: 0.7,
            side: THREE.DoubleSide,
            depthWrite: false,
            normalMap: normalMapTexture,
            normalScale: new THREE.Vector2(0.7, 0.7),
        });

        // keep a reference so material switching can preserve the normal map.
        this.faceMesh = new THREE.Mesh(faceGeo, faceMat);
        this.faceMesh.userData.isTesseractFace = true;
        this.add(this.faceMesh);
    }
}
// Para cada objeto ter:
// como o enunciado diz "Definir dois tipos de materiais para cada objecto da cena: MeshLambertMaterial e MeshPhongMaterial." 
// se formos fazer cada objeto numa classe, então adicionar o metodo switchMaterial à classe
// assim dá para trocar os materiais de cada objeto facilmente e mantê-los com propriedades diferentes na mesma tipo dois phongs diferentes terem cores diferentes for example


function switchActiveModel(value) {
	if(activeModel) {
		scene.remove(activeModel);
	}

	switch(value) {
		case "Nenhum":
			activeModel = null;
			return;

		case "Tesseract":
			activeModel = tesseract;
			break;

		case "Animal":
			activeModel = animal;
			break;

		case "Artemis": 
			activeModel = artemis;
			break;
	}

	// This ensures we're switching the material to the one that is selected in the HUD, otherwise the model would be displayed with its previous selected material
	switchActiveMaterial(hudSettings.material);
	scene.add(activeModel);
}


// Cria variantes Lambert, Phong e Basic para cada mesh do modelo,
function ensureMaterialVariants(model) {
    model.traverse((child) => {
        if (!child.isMesh || child.userData.lambertMaterial) return;

        if (child.userData.isTesseractFace) return;
        const o = child.material;

        child.userData.lambertMaterial = new THREE.MeshLambertMaterial({
            color:         o.color       ? o.color.clone()   : 0xffffff,
            map:           o.map         || null,
            emissive:      o.emissive    ? o.emissive.clone() : 0x000000,
            transparent:   o.transparent || false,
            opacity:       o.opacity     !== undefined ? o.opacity    : 1,
            side:          o.side        !== undefined ? o.side       : THREE.FrontSide,
            depthWrite:    o.depthWrite  !== undefined ? o.depthWrite : true,
        });

        child.userData.phongMaterial = new THREE.MeshPhongMaterial({
            color:         o.color       ? o.color.clone()        : 0xffffff,
            map:           o.map         || null,
            normalMap:     o.normalMap   || null,
            normalScale:   o.normalScale ? o.normalScale.clone()  : new THREE.Vector2(1, 1),
            emissive:      o.emissive    ? o.emissive.clone()     : 0x000000,
            specular:      0xffffff,
            shininess:     120,
            transparent:   o.transparent || false,
            opacity:       o.opacity     !== undefined ? o.opacity    : 1,
            side:          o.side        !== undefined ? o.side       : THREE.FrontSide,
            depthWrite:    o.depthWrite  !== undefined ? o.depthWrite : true,
        });

        child.userData.basicMaterial = new THREE.MeshBasicMaterial({
            color:         o.color       ? o.color.clone()   : 0xffffff,
            map:           o.map         || null,
            transparent:   o.transparent || false,
            opacity:       o.opacity     !== undefined ? o.opacity : 1,
            side:          o.side        !== undefined ? o.side   : THREE.FrontSide,
        });
    });
}

function switchActiveMaterial(materialString) {
    if (!activeModel) return;

    ensureMaterialVariants(activeModel);

    activeModel.traverse((child) => {
        if (!child.isMesh) return;
        if (child.userData.isTesseractFace) return; // preserve normal-mapped material

        if (!hudSettings.iluminacaoAtiva) {
            child.material = child.userData.basicMaterial;
        } else if (materialString === 'Lambert (Gouraud)') {
            child.material = child.userData.lambertMaterial;
        } else {
            child.material = child.userData.phongMaterial;
        }
    });
}
 


////////////////////////
/* CREATE HUD         */
////////////////////////
function createHUD() {
    // A interface deve ser simples e interagir através de cliques/toques no ecrã.
    gui = new GUI({ title: "Comandos Pepper's Ghost" });
    
    // Ocupar o bordo superior direito do ecrã, libertando a zona central para os modelos 3D.
    gui.domElement.style.position = 'absolute';
    gui.domElement.style.top = '10px';
    gui.domElement.style.right = '10px';

    // Botões que permitam alternar entre diferentes modelos 3D.
    const modelosFolder = gui.addFolder('Modelos 3D');
    const modelosController = modelosFolder.add(hudSettings, 'modeloAtual', ['Nenhum', 'Tesseract', 'Animal', 'Artemis']).name('Modelo Ativo');
	modelosController.onChange((value) => { switchActiveModel(value); });

    // Botões para ativar luzes direccional, pontuais e spotlights.
    const luzesFolder = gui.addFolder('Iluminação');
    luzesFolder.add(hudSettings, 'iluminacaoAtiva').name('Ativar Cálculos').onChange(() => {
        switchActiveMaterial(hudSettings.material);
    });
    luzesFolder.add(hudSettings, 'luzDireccional').name('Luz Direcional').onChange((v) => {
        if (directionalLight) directionalLight.visible = v;
    });
    luzesFolder.add(hudSettings, 'luzPontuais').name('Luzes Pontuais').onChange((v) => {
        allPointLights.forEach(l => l.visible = v);
    });
    luzesFolder.add(hudSettings, 'spotlights').name('Spotlights').onChange((v) => {
        allSpotLights.forEach(l => l.visible = v);
    });

    // Botões para alternar entre diferentes materiais.
    const materiaisFolder = gui.addFolder('Materiais');
    const materiaisController = materiaisFolder.add(hudSettings, 'material', ['Lambert (Gouraud)', 'Phong']).name('Tipo de Material');
	materiaisController.onChange((value) => {
		switchActiveMaterial(value);
	});

    // Botão para ativar o efeito anaglífico.
    const visualizacaoFolder = gui.addFolder('Visualização');
    visualizacaoFolder.add(hudSettings, 'efeitoAnaglifico').name('Efeito Anaglífico');

    gui.open();
}

//////////////////////
/* CHECK COLLISIONS */
//////////////////////
function checkCollisions() {}

///////////////////////
/* HANDLE COLLISIONS */
///////////////////////
function handleCollisions() {}

////////////
/* UPDATE */
////////////
function update() {
    const t = clock.getElapsedTime();

    if (tesseract) {
        tesseract.rotation.y = t * 0.6;
        tesseract.rotation.x = t * 0.3;

        const s = 1.0 + 0.25 * Math.sin(t * 1.2);
        tesseract.scale.set(s, s, s);
    }

    if (animal) {
        animal.rotation.y = t * 0.6;
    }

    if (artemis) {
        artemis.rotation.y = t * 0.3;
    }
}

/////////////
/* DISPLAY */
/////////////
function render() {
    renderer.clear();
    if (hudSettings.efeitoAnaglifico) {
        anaglyphEffect.render(scene, camera);
    } else {
        renderer.render(scene, camera);
    }
}

////////////////////////////////
/* INITIALIZE ANIMATION CYCLE */
////////////////////////////////
async function init() {
    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.autoClear = false;
    document.body.appendChild(renderer.domElement);

    anaglyphEffect = new AnaglyphEffect(renderer);
    anaglyphEffect.setSize(window.innerWidth, window.innerHeight);

    await createScene();
    createCamera();
    createHUD();

    window.addEventListener('resize', onResize);

	// Needs to be here because init is async and we don't want animate() to be called before init finishes
	animate();
}

/////////////////////
/* ANIMATION CYCLE */
/////////////////////
function animate() {
    requestAnimationFrame(animate);
    update();
    render();
}

////////////////////////////
/* RESIZE WINDOW CALLBACK */
////////////////////////////
function onResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
    anaglyphEffect.setSize(window.innerWidth, window.innerHeight);
}

///////////////////////
/* KEY DOWN CALLBACK */
///////////////////////
function onKeyDown(e) {}

///////////////////////
/* KEY UP CALLBACK */
///////////////////////
function onKeyUp(e) {}

init();