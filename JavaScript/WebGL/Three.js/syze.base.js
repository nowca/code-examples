SYZE.renderer = null;
SYZE.viewport = null;
SYZE.stats    = null;
SYZE.camera   = null;
SYZE.scene    = null;

SYZE.objects  = {
    intersected: null,
    selected:    null,
    moved:       null,

    table:       null,
    modules:     [],

    // move object smoothly to position of target
    drag: function( object, target )
    {
        var easingAmount = 0.3;
        var distance     = SYZE.utils.equation.distance( object, target );

        if (distance.value > 1)
        {
            object.x += distance.deltaX * easingAmount;
            object.y += distance.deltaY * easingAmount;
        }
    },

    // get app data
    getModuleMeshes: function()
    {
        var moduleMeshes = [];

        for ( var i in SYZE.objects.modules )
        {
            var module = SYZE.objects.modules[i];

            {
                module.name = module.name == ''                               ?
                              module.type + '-' + SYZE.objects.modules.length :
                              module.name                                     ;
            }

            moduleMeshes.push( SYZE.objects.modules[i].glData.mesh );
        } 

        return moduleMeshes;
    }
};

//// app main loop /////////////////////////////////////////////////////////////
SYZE.run      = function()
{
    // trigger update event
    window.dispatchEvent( SYZE.update );

    // render scene
    SYZE.renderer.render( SYZE.scene, SYZE.camera );

    // request next run iteration
    window.requestAnimationFrame( SYZE.run );
};


//// app events ////////////////////////////////////////////////////////////////
SYZE.update   = new CustomEvent( "update", { type: 'update' } );


//// app loading ///////////////////////////////////////////////////////////////
SYZE.start    = function()
{
    // initialize application
    SYZE.init();

    // create table
    SYZE.setTable( new SYZE.Table() );

    // create new sine module
    var sine_module  = new SYZE.OscModule();
    sine_module.glData.mesh.position.set(0, 0, 0);


    var sine_module2  = new SYZE.OscModule();
    sine_module2.glData.mesh.position.x -= 10;

    var sine_module3  = new SYZE.OscModule();
    sine_module3.glData.mesh.position.x += 10;


    // add modules to table
    SYZE.addModule( sine_module );
    SYZE.addModule( sine_module2 );
    SYZE.addModule( sine_module3 );

    // turn on the light
    SYZE.setLight();


    SYZE.modulecontrol = {};


    var geometry = new THREE.TorusGeometry( 10, 1, 2, 100, Math.PI * 2 );

    var materials = [
        new THREE.MeshBasicMaterial( { color: 0x5bc9ff, transparent: true, opacity: 0.8, depthWrite:  false } ),
        new THREE.MeshBasicMaterial( { color: 0x5bc9ff, transparent: true, opacity: 0.1, depthWrite:  false } )
    ];

    for( var i = 0; i < geometry.faces.length; i++ )
    {
        geometry.faces[ i ].materialIndex = ( i < 150 && i > 49) ? 0: 1;
    }


    var torus = new THREE.Mesh( geometry, new THREE.MeshFaceMaterial( materials )  );

    torus.scale.set(2, 2, 2);
    torus.position.x += 50;
    torus.position.z  = -25;

    SYZE.modulecontrol.torus = torus;

    SYZE.modulecontrol.torus.renderOrder = 1;


    SYZE.scene.add( torus );;


    // create a canvas element
    var canvas1 = document.createElement('canvas');
    canvas1.width  = 1024;
    canvas1.height = 1024;
    var context1 = canvas1.getContext('2d');
    context1.font = "10px dotted";
    context1.fillStyle = "rgba(91,201,255,1.0)";
    context1.fillText('Sine Module 2', 600, 420);
    context1.fillText('50 %', 600, 620);

    // canvas contents will be used for a texture
    var texture1 = new THREE.Texture(canvas1);
    texture1.needsUpdate = true;

    var material1 = new THREE.MeshBasicMaterial({
        map:         texture1,
        side:        THREE.DoubleSide,
        blending:    THREE.AdditiveBlending,
        transparent: true,
        opacity:     0.8,
        depthWrite:  false 
    });


    var mesh1 = new THREE.Mesh(
        new THREE.PlaneGeometry(200, 200),
        material1
      );
    mesh1.position.x = torus.position.x;
    mesh1.position.y = torus.position.y;
    mesh1.position.z = -20.0;


    SYZE.modulecontrol.text = mesh1;

    SYZE.modulecontrol.text.renderOrder = 1;

    SYZE.scene.add( mesh1 );

    SYZE.run();
};


SYZE.init     = function()
{
    SYZE.initDocument();
    SYZE.initRenderer();
    SYZE.initUI();                   
    SYZE.initScene();                    
    SYZE.initCamera();                    
    SYZE.initControls();                    
    SYZE.initTimer();
    SYZE.setEvents();
};


//// app component setters /////////////////////////////////////////////////////
SYZE.initDocument = function()
{
    SYZE.utils.console.printInfo();
    document.title = "SYNTHONIZE - Web Audio Soundground";
};

SYZE.initRenderer = function()
{
    // initialize THREE WebGLRenderer
    SYZE.renderer = new THREE.WebGLRenderer( { antialias: true } );
    //SYZE.renderer.sortObjects = false;
    SYZE.renderer.setSize( SYZE.settings.graphics.width, SYZE.settings.graphics.height );

    if ( SYZE.settings.graphics.shadows )
    {
        SYZE.renderer.shadowMap.enabled = true;
        SYZE.renderer.shadowMap.type    = THREE.PCFSoftShadowMap;
    }

    // append webgl viewport to dom
    SYZE.viewport = document.getElementById( 'SYNTHONIZE' );
    SYZE.viewport.appendChild( SYZE.renderer.domElement );
};

SYZE.initUI       = function()
{
    // initialize Stats plugIn
    if ( SYZE.settings.debug.stats )
    {
        SYZE.stats = new Stats();
        SYZE.stats.domElement.style.position = 'absolute';
        SYZE.stats.domElement.style.top      = '0px';
        SYZE.viewport.appendChild( SYZE.stats.domElement );
    } 
};

SYZE.initScene    = function()
{
    // initialize THREE Scene
    SYZE.scene = new THREE.Scene();
};

SYZE.initCamera   = function()
{
    // initialize camera
    SYZE.settings.camera.aspect = SYZE.settings.graphics.width / SYZE.settings.graphics.height;

    SYZE.camera   = new THREE.PerspectiveCamera(
        SYZE.settings.camera.view_angle,
        SYZE.settings.camera.aspect,
        SYZE.settings.camera.near,
        SYZE.settings.camera.far
    );

    SYZE.camera.position.z = 130;
    SYZE.camera.up         = new THREE.Vector3(0,0,1);
    SYZE.scene.add( SYZE.camera );
};

SYZE.initControls = function()
{
    // setup mouse tracker
    SYZE.control.raycaster      = new THREE.Raycaster( SYZE.camera.position );
    SYZE.control.mouse.vector   = new THREE.Vector2();
    SYZE.control.mouse.position = { x: 0, y: 0 };
    SYZE.control.mouse.speed    = {
        x:        0,
        y:        0,
        t:        0,
        ic:       0,
        lastX:    0,
        lastY:    0,
        interval: 0
    };

    // initialize TrackballControls plugIn
    if ( SYZE.settings.debug.trackball )
    {
        SYZE.control.trackball              = new THREE.TrackballControls( SYZE.camera );
        SYZE.control.trackball.rotateSpeed  = 1.0;
        SYZE.control.trackball.zoomSpeed    = 1.0;
        SYZE.control.trackball.panSpeed     = 1.0;
        SYZE.control.trackball.noZoom       = false;
        SYZE.control.trackball.noPan        = false;
        SYZE.control.trackball.noRotate     = false;
        SYZE.control.trackball.staticMoving = false;
        SYZE.control.trackball.keys         = [ 65, 83, 68 ];
    }
};

SYZE.initTimer    = function()
{
    // start internal timer
    SYZE.timer.run();
};



SYZE.setEvents    = function()
{
    // setup event listeners
    window.addEventListener( 'mousemove', SYZE.events.onMouseMove,    false );
    window.addEventListener( 'mouseup',   SYZE.events.onMouseUp,      false );
    window.addEventListener( 'mousedown', SYZE.events.onMouseDown,    false );
    window.addEventListener( 'keyup',     SYZE.events.onKeyUp,        false );
    window.addEventListener( 'keydown',   SYZE.events.onKeyDown,      false );
    window.addEventListener( 'resize',    SYZE.events.onWindowResize, false );
    window.addEventListener( 'update',    SYZE.events.onUpdate,       false );
};

SYZE.setLight     = function()
{       
    var spotLight = new THREE.SpotLight( 0xccccff );
    spotLight.target.position.set( 0, 0, 0 );
    spotLight.position.x = 0;
    spotLight.position.y = 0;
    spotLight.position.z = 130;

    spotLight.shadowCameraNear	  = 0.1;	
    spotLight.shadowCameraFar     = 170;
    spotLight.castShadow          = true;
    spotLight.shadowDarkness      = 0.2;

    SYZE.objects.table.spotlight = spotLight;
    SYZE.scene.add( spotLight );
};

SYZE.setTable     = function( table )
{
    // set apps table
    SYZE.objects.table = table;

    // add object
    SYZE.scene.add( table.glData.mesh );
    SYZE.scene.add( table.beatlight.glData.mesh );
    SYZE.scene.add( table.edgelight.glData.mesh );
};



SYZE.addModule    = function( module )
{
    // add module to app objects
    this.objects.modules.push( module );

    module.name = module.type.toLowerCase() + '_' + this.objects.modules.length;

    // add module mesh to scene
    SYZE.scene.add( module.glData.mesh );
    SYZE.scene.add( module.sublight.glData.mesh );
};