const char HEGwebAPI[] PROGMEM = R"=====(
  class HEGwebAPI {
    constructor(defaultUI=true, parentId="main_body"){
      this.ms=[];
      this.red=[];
      this.ir=[];
      this.ratio=[];
      this.smallSavLay=[];
      this.largeSavLay=[];
      this.adcAvg=[];
      this.ratioSlope=[];
      this.AI=[];

      this.slowSMA = 0;
      this.fastSMA = 0;
      this.smaSlope = 0;
      this.scoreArr = [0];
      this.replay = false;

      this.csvDat = [];
      this.csvIndex = 0;

      this.sensitivity = null;
      if(defaultUI==true){
        this.createHegUI(parentId);
      }
    }

    //appendId is the element Id you want to append this fragment to
    static appendFragment(HTMLtoAppend, parentId) {

        var fragment = document.createDocumentFragment();
        var newDiv = document.createElement('div');
        newDiv.innerHTML = HTMLtoAppend;
        newDiv.setAttribute("id", parentId + '_child');

        fragment.appendChild(newDiv);

        document.getElementById(parentId).appendChild(fragment);
    }

    //delete selected fragment. Will delete the most recent fragment if Ids are shared.
    static deleteFragment(parentId,fragmentId) {
        var this_fragment = document.getElementById(fragmentId);
        document.getElementById(parentId).removeChild(this_fragment);
    }

    //Separates the appendId from the fragmentId so you can make multiple child threads with different Ids
    static appendFragmentMulti(HTMLtoAppend, parentId, fragmentId) {

        var fragment = document.createDocumentFragment();
        var newDiv = document.createElement('div');
        newDiv.innerHTML = HTMLtoAppend;
        newDiv.setAttribute("id", parentId + '_child');

        fragment.appendChild(newDiv);

        document.getElementById(parentId).appendChild(fragment);
    }

    resetVars() {
        this.ms = [];
        this.red = [];
        this.ir = [];
        this.ratio = [];
        this.smallSavLay = [];
        this.largeSavLay = [];
        this.adcAvg = [];
        this.ratioSlope = [];
        this.AI = [];
        
        this.slowSMA = 0;
        this.fastSMA = 0;
        this.smaSlope = 0;
        this.scoreArr = [0];
        this.replay = false;
    }

    smaScore() {
      var temp = this.largeSavLay.slice(this.largeSavLay.length - 40,this.largeSavLay.length);
      var temp2 = this.largeSavLay.slice(this.largeSavLay.length - 20,this.largeSavLay.length);
      this.slowSMA = temp.reduce((a,b) => a + b, 0) / 40;
      this.fastSMA = temp2.reduce((a,b) => a + b, 0) / 20;
      this.smaSlope = this.fastSMA - this.slowSMA;
      this.scoreArr.push(this.smaSlope);
    }

    saveCSV(){
      var csv = "ms,red,ir,ratio,sSavLay,lSavLay,adcAvg,ratioSlope,AI\n"; //csv header
      for(var i = 0; i<this.ms.length - 1; i++){
        var temp = [this.ms[i],this.red[i],this.ir[i],this.ratio[i],this.smallSavLay[i],this.largeSavLay[i],this.adcAvg[i],this.ratioSlope[i],this.AI[i],].join(',') + "\n";
        csv += temp;
      }
      var hiddenElement = document.createElement('a');
      hiddenElement.href = "data:text/csv;charset=utf-8," + encodeURI(csv);
      hiddenElement.target = "_blank";
      hiddenElement.download = document.getElementById("csvname").value+".csv";
      hiddenElement.click();
    }
 
    replayCSV = () => {
      console.log("Define replay function in script");
    }
    
    openCSV() {
      var input = document.createElement('input');
      input.type = 'file';

      input.onchange = e => {
        this.csvDat = [];
        this.csvIndex = 0;
        var file = e.target.files[0];
        var reader = new FileReader();
        reader.readAsText(file);
        reader.onload = event => {
          this.resetVars();
          var tempcsvData = event.target.result;
          var tempcsvArr = tempcsvData.split("\n");
          tempcsvArr.pop();
          tempcsvArr.forEach((row,i) => {
            if(i==0){ var temp = row.split(","); }
            else{
              var temp = row.split(",");
              this.csvDat.push(temp);
            }
          });
          this.replay = true;
          this.replayCSV();
        }
        input.value = '';
      }
      input.click();
    }

    handleData(e){
      console.log("HEGDUINO", e.data);
    }

    createEventListeners() {
      if (!!window.EventSource) {
        var source = new EventSource('/events');

        source.addEventListener('open', function(e) {
            console.log("HEGDUINO", "Events Connected");
            //document.getElementById("message").innerHTML = "Output:";
        }, false);

        source.addEventListener('error', function(e) {
            if (e.target.readyState != EventSource.OPEN) {
              console.log("HEGDUINO", "Events Disconnected");
            }
        }, false);

        source.addEventListener('message', function(e) {
            document.getElementById("message").innerHTML = e.data;
            console.log("HEGDUINO", e.data);
        }, false);

        source.addEventListener('heg', (e) => {
            this.handleData(e);
        }, false);
      }
    }
   
    updateTable(){
      document.getElementById("dataTable").innerHTML = '<tr><td id="ms">'+this.ms[this.ms.length-1-1]+'</td><td id="red">'+this.red[this.red.length-1-1]+'</td><td id="ir">'+this.ir[this.ir.length-1-1]+'</td><td id="ratio">'+this.ratio[this.ratio.length-1-1]+'</td><td id="smallSavLay">'+this.smallSavLay[this.smallSavLay.length-1-1]+'</td><td id="largeSavLay">'+this.largeSavLay[this.largeSavLay.length-1-1]+'</td><td id="adcAvg">'+this.adcAvg[this.adcAvg.length-1-1]+'</td><td id="ratioSlope">'+this.ratioSlope[this.ratioSlope.length-1-1]+'</td><td id="AI">'+this.AI[this.AI.length-1]+'</td><td class="scoreth">'+this.scoreArr[this.scoreArr.length-1].toFixed(4)+'</td></tr>';
    }

    createHegUI(parentId) {
      var hegapiHTML = '<div id="hegapi" class="hegapi"> \
        <form method="post" action="/startHEG" target="dummyframe"><button id="startbutton" class="button startbutton" type="submit">Start HEG</button></form> \
        <form method="post" action="/stopHEG" target="dummyframe"><button id="stopbutton" class="button stopbutton" type="submit">Stop HEG</button></form> \
        <form class="sendcommand" method="post" action="/command" target="dummyframe"><label class="label" for="command">Command:</label><br><input type="text" id="command" name="command"><button class="button sendbutton" type="submit">Send</button></form> \
        <div id="saveLoad" class="saveLoadBar"> \
          <label class="label" for="csvname">Save Session:</label><br><input type="text" id="csvname" name="csvname" placeholder="session_data" required></input> \
          <button class="button saveLoadButtons" id="savecsv">Save CSV</button> \
          <button class="button saveLoadButtons" id="replaycsv">Replay CSV</button> \
        </div> \
        <div id="sensitivityBar" class="sensBar"> \
          Sensitivity:<br><input type="range" id="sensitivity" min="1" max="200" value="100"> \
          <button class="button" id="reset_s">Default</button> \
        </div> \
        </div> \
        <iframe name="dummyframe" id="dummyframe" class="dummy"></iframe> \
        ';
  
      var dataDivHTML = '<dataDiv id="dataDiv"></dataDiv>';
      var containerHTML = '<div id="container"></div>';
      var messageHTML = '<msgDiv id="message">Output:</div>';
      var eventHTML = '<eventDiv id="heg">Not connected...</eventDiv>';
      var tableHeadHTML = '<div id="tableHead"><table class="dattable" id="dataNames"><tr><th>ms</th><th>Red</th><th>IR</th><th>Ratio</th><th>sSavLay</th><th>lSavLay</th><th>adcAvg</th><th>rSlope</th><th>A.I.</th><th class="scoreth">SMA1s-2s</th></tr></table></div>';
      var tableDatHTML = '<div id="tableDat"><table class="dattable" id="dataTable"><tr><th>Awaiting Data...</th></tr></table></div>';

      HEGwebAPI.appendFragment(dataDivHTML,"main_body");
      HEGwebAPI.appendFragment(hegapiHTML,"main_body");
      HEGwebAPI.appendFragment(containerHTML,"dataDiv");
      HEGwebAPI.appendFragment(messageHTML,"container");
      HEGwebAPI.appendFragment(eventHTML,"container");
      HEGwebAPI.appendFragment(tableHeadHTML,"container");
      HEGwebAPI.appendFragment(tableDatHTML,"container");

      document.getElementById("startbutton").onclick = () => { this.resetVars(); }
      document.getElementById("savecsv").onclick = () => {this.saveCSV();}
      document.getElementById("replaycsv").onclick = () => {this.openCSV();}
      this.sensitivity = document.getElementById("sensitivity");
      document.getElementById("reset_s").onclick = () => { this.sensitivity.value = 100; }
      
      this.createEventListeners();
    }
  }


  class graphJS {
    constructor(parentId, canvasId="graph", nPoints=[1000], color=[0,255,0,1], defaultUI=true, res=[1400,400]){
      //WebGL graph based on: https://tinyurl.com/y5roydhe
      //HTML : <canvas id={canvasId}></canvas><canvas id={canvasId+"text"}></canvas>;
      this.gl,
      this.shaderProgram,
      this.vertices,
      this.canvas;

      this.parentId = parentId;
      this.canvasId = canvasId;
      this.textId = canvasId + "text";
      this.color = color;
      this.res = res;
            
      this.ms = [0];
      this.VERTEX_LENGTH = nPoints;
      this.graphY1 = [...Array(this.VERTEX_LENGTH).fill(0)];

      this.yscale = 1;
      this.invScale = 1/this.yscale;
      this.offset = 0; //Index offset
      
      this.xAxis = new Float32Array([
      -1.0,0.0,
      1.0,0.0
      ]);

      this.yAxis = new Float32Array([
      -0.8,-1.0,
      -0.8,1.0
      ]);

      this.gradient = new Float32Array([
      -1.0,-0.75,
      1.0,-0.75,
      -1.0,-0.5,
      1.0,-0.5,
      -1.0,-0.25,
      1.0,-0.25,
      -1.0,0.25,
      1.0,0.25,
      -1.0,0.5,
      1.0,0.5,
      -1.0,0.75,
      1.0,0.75
      ]);

      this.VERTEX_SHADER = `
        attribute vec4 coords;
        void main(void) {
          gl_Position = coords;
        }
      `;

      this.FRAGMENT_SHADER = `
        precision mediump float;
        uniform vec4 color;
        void main(void) {
          gl_FragColor = color;
        }
      `;

      if(defaultUI == true){
        this.createUI(parentId);
      }
      this.initGL(canvasId);
      this.createShader();
      this.createVertices(color);
      
      this.graphtext = document.getElementById(this.textId).getContext("2d");
      this.graphtext.canvas.width = res[0];
      this.graphtext.canvas.height = res[1];
      this.graphtext.font = "20pt Arial";
      
      this.draw();

      window.addEventListener('resize', this.setCanvasSize(this.canvasId), false);
    }

    createUI(parentId){
      var shaderHTML = '<div id="shaderContainer"> \
      <canvas class="webglcss" id="'+this.canvasId+'"></canvas><canvas class="webglcss" id="'+this.canvasId+'text"></canvas> \
      <div class="scale"> \
        X Offset:<br><input type="range" id="xoffset" min=0 max=1000 value=0><button id="xoffsetbutton" class="button">Reset</button><br> \
        X Scale:<br><input type="range" id="xscale" min=10 max=3000 value=1000><button id="xscalebutton" class="button">Reset</button><br> \
        Y Scale:<br><input type="range" id="yscale" min=1 max=1000 value=100><button id="yscalebutton" class="button">Reset</button> \
      </div> \
      </div> \
      ';

      HEGwebAPI.appendFragment(shaderHTML,parentId);

      this.xoffsetSlider = document.getElementById("xoffset");

      this.xscaleSlider = document.getElementById("xscale");
      
      this.yscaleSlider = document.getElementById("yscale");
      this.yscaleSlider.oninput = () => {
        this.yscale = this.yscaleSlider.value * .01;
        this.invScale = 1/this.yscale;
      }
      document.getElementById("yscalebutton").onclick = () => {
        this.yscaleSlider.value = 100;
        this.yscale = 1;
        this.invScale = 1;
      }
    }

    setCanvasSize(canvasId) {
      this.canvas.width = this.res[0]; //Define in CSS
      this.canvas.height = this.res[1];
      this.gl.viewport(0, 0, this.gl.drawingBufferWidth, this.gl.drawingBufferHeight);
    }

    initGL(canvasId) {
      this.canvas = document.querySelector('#'+canvasId);
      this.gl = this.canvas.getContext('webgl');
        this.setCanvasSize(canvasId);
        console.log(this.gl.drawingBufferWidth, this.gl.drawingBufferHeight);
        this.gl.viewport(0, 0, this.gl.drawingBufferWidth, this.gl.drawingBufferHeight);
        this.gl.enable(this.gl.BLEND);
        this.gl.blendFunc(this.gl.SRC_ALPHA, this.gl.ONE_MINUS_SRC_ALPHA);
        this.gl.clearColor(0, 0, 0, 1);
    }
      
    makePoints(numPoints, yArr) {
      const highestPointNdx = numPoints - 1;
      return Array.from({length: numPoints * 2}, (_, i) => {
        const pointId = i / 2 | 0;
        const lerp0To1 = pointId / highestPointNdx;
        const isY = i % 2;
        return isY
          ? yArr[i]*this.yscale // Y
          : (lerp0To1 * 4 - 1); // X
      });
    }

    createVertices(vArr,color=this.color) {
      const buffer = this.gl.createBuffer();
      this.gl.bindBuffer(this.gl.ARRAY_BUFFER, buffer);
      this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array(vArr), this.gl.DYNAMIC_DRAW);

      const coords = this.gl.getAttribLocation(this.shaderProgram, 'coords');
      this.gl.vertexAttribPointer(coords, 2, this.gl.FLOAT, false, 0, 0);
      this.gl.enableVertexAttribArray(coords);
      // gl.bindBuffer(gl.ARRAY_BUFFER, null);

      const uniformColor = this.gl.getUniformLocation(this.shaderProgram, 'color');
      this.gl.uniform4f(uniformColor, this.normalize(color[0]), this.normalize(color[1]), this.normalize(color[2]), color[3]);
    }

    createShader() {
      const vs = this.VERTEX_SHADER;

      const vertexShader = this.gl.createShader(this.gl.VERTEX_SHADER);
      this.gl.shaderSource(vertexShader, vs);
      this.gl.compileShader(vertexShader);

      const fs = this.FRAGMENT_SHADER;

      const fragmentShader = this.gl.createShader(this.gl.FRAGMENT_SHADER);
      this.gl.shaderSource(fragmentShader, fs);
      this.gl.compileShader(fragmentShader);

      this.shaderProgram = this.gl.createProgram();
      this.gl.attachShader(this.shaderProgram, vertexShader);
      this.gl.attachShader(this.shaderProgram, fragmentShader);

      this.gl.linkProgram(this.shaderProgram);
      this.gl.useProgram(this.shaderProgram);
    }

    draw = () => {
      //Create main graph
      this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);

      //xAxis
      this.createVertices(this.xAxis,[255,255,255,0.8]);
      this.gl.drawArrays(this.gl.LINES, 0, 2);

      //yAxis
      this.createVertices(this.yAxis, [255,255,255,0.8]);
      this.gl.drawArrays(this.gl.LINES, 0, 2);

      //gradient
      this.createVertices(this.gradient, [70,70,70,0.5]);
      this.gl.drawArrays(this.gl.LINES, 0, 12);
      
      //Data line
      this.vertices = this.makePoints(this.VERTEX_LENGTH, this.graphY1);
      this.createVertices(this.vertices);
      this.gl.bufferSubData(this.gl.ARRAY_BUFFER, 0, new Float32Array(this.vertices));
      this.gl.drawArrays(this.gl.LINE_STRIP, 0, this.VERTEX_LENGTH);

      //Create text overlay
      this.graphtext.clearRect(0, 0, this.canvas.width, this.canvas.height);
      this.graphtext.fillStyle = "#00ff00";
      this.graphtext.fillText("t: " + (this.ms[this.ms.length - 1]*0.001).toFixed(2),5,25);
      this.graphtext.fillText("y: " + this.graphY1[this.graphY1.length - 1].toFixed(4),5,50);
      this.graphtext.fillStyle = "#707070";
      var xoffset = this.graphtext.canvas.width * 0.11;
      this.graphtext.fillText((this.invScale * 0.75).toFixed(3), xoffset, this.graphtext.canvas.height * 0.125); 
      this.graphtext.fillText((this.invScale * 0.5).toFixed(3), xoffset, this.graphtext.canvas.height * 0.25); 
      this.graphtext.fillText((this.invScale * 0.25).toFixed(3), xoffset, this.graphtext.canvas.height * 0.375); 
      this.graphtext.fillText((this.invScale * -0.25).toFixed(3), xoffset, this.graphtext.canvas.height * 0.625); 
      this.graphtext.fillText((this.invScale * -0.5).toFixed(3), xoffset, this.graphtext.canvas.height * 0.75); 
      this.graphtext.fillText((this.invScale * -0.75).toFixed(3), xoffset, this.graphtext.canvas.height * 0.875); 
      requestAnimationFrame(this.draw);
    }

    normalize(val, max=255, min=0) { return (val - min) / (max - min); }
  }
      
  class circleJS {
    constructor(parentId, canvasId="circlecanvas", defaultUI=true){
      this.parentId = parentId;
      if(defaultUI == true){
        this.createCanvas(parentId, canvasId);
      }
      this.c = document.getElementById(canvasId);
      this.ctx = this.c.getContext('2d');
       
      this.cWidth = this.c.width;
      this.cHeight = this.c.height;
 
      this.angle = 1.57;
      this.angleChange = 0;

      this.draw();
    }

    createCanvas(parentId,canvasId){
      var canvasHTML = '<div id="canvasContainer"><canvas class="canvascss" id="'+canvasId+'" height="400px" width="400px"></canvas></div>'

      //Setup page as fragments so updates to elements are asynchronous.
      HEGwebAPI.appendFragment(canvasHTML,parentId);
    }

    onData(e) {
      
    }

    onNoData(e) {
      
    }

    draw = () => {
        this.ctx.clearRect(0, 0, this.cWidth, this.cHeight);
         
        // color in the background
        this.ctx.fillStyle = "#2751CE";
        this.ctx.fillRect(0, 0, this.cWidth, this.cHeight);
         
        // draw the circle
        this.ctx.beginPath();
         
        var radius = 25 + 175 * Math.abs(Math.cos(this.angle));
        this.ctx.arc(200, 200, radius, 0, Math.PI * 2, false);
        this.ctx.closePath();
         
        // color in the circle
        this.ctx.fillStyle = "#EE1818";
        this.ctx.fill();

        if(((this.angle > 1.57) || (s.smaSlope > 0)) && ((this.angle < 3.14) || (s.smaSlope < 0))) {
          this.angle += this.angleChange;
        }
        requestAnimationFrame(this.draw);
    }
  }

    class videoJS {
        constructor(parentId, vidapiId="vidapi", vidContainerId="vidbox", defaultUI=true){
          this.playRate = 1;
          this.alpha = 0;
          this.useAlpha = true;
          this.useRate = true;
          this.enableControls = false;
          this.parentId = parentId;
          this.vidapiId = vidapiId
          this.vidContainerId = vidContainerId

          this.vidQuery;
          this.c;
          this.gl;
          if(defaultUI=true){
            this.addUI(this.parentId);
          }
          this.init();
        }

        setupButtons() {
          document.getElementById("startbutton").onclick = () => {
            if(this.playRate < 0.1){ this.vidQuery.playbackRate = 0; }
            else{ this.vidQuery.playbackRate = this.playRate; }
          }
          document.getElementById("stopbutton").onclick = () => {this.vidQuery.playbackRate = 0;}
        }

        deInit(){
          document.getElementById("startbutton").onclick = function(){
            return;
          }
          document.getElementById("stopbutton").onclick = function(){
            return;
          }
        }

        addUI(parentId){
         var videoapiHTML = '<div id="'+this.vidapiId+'" style="position:absolute; top:200px; right:0px;"> \
          <input class="button" id="fs" name="fs" type="file" accept="video/*"/><br> \
          <button class="button" id="useAlpha" name="useAlpha">Fade</button> \
          <button class="button" id="useRate" name="useRate">Speed</button> \
          </div>';
         var videoHTML = '<div id="'+this.vidContainerId+'"><video id="'+this.vidContainerId+'vid" height="480px" width="640px" class="canvascss" src="https://vjs.zencdn.net/v/oceans.mp4" type="video/mp4" autoplay loop muted></video><canvas class="canvascss" id="'+this.vidContainerId+'canvas"></canvas></div>';
         HEGwebAPI.appendFragment(videoapiHTML,parentId);
         HEGwebAPI.appendFragment(videoHTML,parentId);
         this.localFileVideoPlayer();
        }

       localFileVideoPlayer() {
        'use strict'
        var URL = window.URL || window.webkitURL;
        var displayMessage = function (message, isError) {
          var element = document.querySelector('#message');
          element.innerHTML = message;
          element.className = isError ? 'error' : 'info';
        }
        var playSelectedFile = function (event) {
          var file = this.files[0];
          var type = file.type;
          var videoNode = document.querySelector('video');
          var canPlay = videoNode.canPlayType(type);
          if (canPlay === ''){ canPlay = 'no';}
          var message = 'Can play type "' + type + '": ' + canPlay;
          var isError = canPlay === 'no';
          displayMessage(message, isError)
          if (isError) {
            return;
          }
          var fileURL = URL.createObjectURL(file);
          videoNode.src = fileURL;
        }
        var inputNode = document.querySelector('input[name="fs"]');
        inputNode.addEventListener('change', playSelectedFile, false);
      }

      onData(score){
        if(this.useAlpha == true) {
          if(((this.alpha < 0.8) || (score > 0)) && ((this.alpha > 0)||(score < 0))){
            if(this.alpha - score < 0){
              this.alpha = 0;
            }
            else if(alpha - score > 0.8){
              this.alpha = 0.8;
            }
            else{
              this.alpha -= score;
            }
          }
        }
        if(this.useRate == true){
          if(((this.vidQuery.playbackRate < 3) || (score < 0)) && ((this.vidQuery.playbackRate > 0) || (score > 0)))
          { 
            this.playRate = this.vidQuery.playbackRate + score*0.5;
            if((this.playRate < 0.05) && (this.playRate > 0)){
              this.vidQuery.playbackRate = 0;
            }
            else if(playRate < 0) {
              this.vidQuery.currentTime += score;
            }
            else if((this.playRate > 0.05) && (this.playRate < 0.1)){
              this.vidQuery.playbackRate = 0.5;
            }
            else{
              this.vidQuery.playbackRate = this.playRate;
            }
          }
        }
      }
      
      animateRect = () => {
          this.gl.clearColor(0,0,0.1,this.alpha);
          this.gl.clear(this.gl.COLOR_BUFFER_BIT);
          requestAnimationFrame(this.animateRect);
      }

      init() {
         this.vidQuery = document.getElementById(this.vidContainerId+'vid');
         this.c = document.getElementById(this.vidContainerId+'canvas');
         this.c.width = this.vidQuery.width;
         this.c.height = this.vidQuery.height;
         this.gl = this.c.getContext("webgl");
         this.gl.clearColor(0,0,0.1,0);
         this.gl.clear(this.gl.COLOR_BUFFER_BIT);

         this.setupButtons();

         this.animateRect();

         document.getElementById("useAlpha").onclick = () => {
          if(useAlpha == true){
            this.useAlpha = false;
            this.alpha = 0;
          }
          else{ this.useAlpha = true; }
         }

         document.getElementById("useRate").onclick = () => {
          if(useRate == true){
            this.useRate = false;
            this.playRate = 1;
            this.vidQuery.playbackRate = 1;
          }
          else{ this.useRate = true; }
         }
       }
     }
)=====";