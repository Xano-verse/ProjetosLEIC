// Bake-off #2 -- Seleção em UIs Densas
// IPM 2024-25, Período 3
// Entrega: até às 23h59, dois dias úteis antes do sétimo lab (via Fenix)
// Bake-off: durante os laboratórios da semana de 31 de Março

// p5.js reference: https://p5js.org/reference/

// Database (CHANGE THESE!)
const GROUP_NUMBER        = 58;      // Add your group number here as an integer (e.g., 2, 3)
const RECORD_TO_FIREBASE  = false;  // Set to 'true' to record user results to Firebase

// Pixel density and setup variables (DO NOT CHANGE!)
let PPI, PPCM;
const NUM_OF_TRIALS       = 12;     // The numbers of trials (i.e., target selections) to be completed
let continue_button;
let legendas;                       // The item list from the "legendas" CSV

// Metrics (DO NOT CHANGE!)
let testStartTime, testEndTime;     // time between the start and end of one attempt (8 trials)
let hits 			      = 0;      // number of successful selections
let misses 			      = 0;      // number of missed selections (used to calculate accuracy)
let database;                       // Firebase DB  

// Study control parameters (DO NOT CHANGE!)
let draw_targets          = false;  // used to control what to show in draw()
let trials;                         // contains the order of targets that activate in the test
let current_trial         = 0;      // the current trial number (indexes into trials array above)
let attempt               = 0;      // users complete each test twice to account for practice (attemps 0 and 1)

// Target list and layout variables
let targets               = [];
const GRID_ROWS           = 8;      // We divide our 80 targets in a 8x10 grid
const GRID_COLUMNS        = 16;     // We divide our 80 targets in a 8x10 grid

// Ensures important data is loaded before the program starts
function preload()
{
  // id,name,...
  legendas = loadTable('legendas/G_'+GROUP_NUMBER+'.csv', 'csv', 'header');
  //console.log(legendas);
  //sound = loadSound("nintendo-game-boy-startup.mp3");
  sound = loadSound("applepay.mp3");
  //  console.log("Legendas Table:", legendas);
 // console.log("Row Count:", legendas.getRowCount());
  //sortLegendasAlphabetically();
}

// Runs once at the start
function setup()
{
  createCanvas(700, 700);    // window size in px before we go into fullScreen()
  frameRate(60);             // frame rate (DO NOT CHANGE!)
  
  randomizeTrials();         // randomize the trial order at the start of execution
  drawUserIDScreen();        // draws the user start-up screen (student ID and display size)
}

// Runs every frame and redraws the screen
function draw()
{
  if (draw_targets && attempt < 2)
  {     
    // The user is interacting with the 6x3 target grid
    background(color(0,0,0));        // sets background to black
    
    // Print trial count at the top left-corner of the canvas
    textFont("Arial", 16);
    fill(color(255,255,255));
    textAlign(LEFT);
    text("Trial " + (current_trial + 1) + " of " + trials.length, 50, 20);
    
    //Target.prototype.drawCircles(targets);
    
    // Draw all targets
	for (var i = 0; i < legendas.getRowCount(); i++) targets[i].draw();
    
    // Draws the target label to be selected in the current trial. We include 
    // a black rectangle behind the trial label for optimal contrast in case 
    // you change the background colour of the sketch (DO NOT CHANGE THESE!)
    fill(color(0,0,0));
    rect(0, height - 40, width, 40);
 
    textFont("Arial", 20); 
    fill(color(255,255,255)); 
    textAlign(CENTER); 
    text(legendas.getString(trials[current_trial],1), width/2, height - 20);
    

  }
}

// Print and save results at the end of 12 trials
function printAndSavePerformance()
{
  // DO NOT CHANGE THESE! 
  let accuracy			= parseFloat(hits * 100) / parseFloat(hits + misses);
  let test_time         = (testEndTime - testStartTime) / 1000;
  let time_per_target   = nf((test_time) / parseFloat(hits + misses), 0, 3);
  let penalty           = constrain((((parseFloat(95) - (parseFloat(hits * 100) / parseFloat(hits + misses))) * 0.2)), 0, 100);
  let target_w_penalty	= nf(((test_time) / parseFloat(hits + misses) + penalty), 0, 3);
  let timestamp         = day() + "/" + month() + "/" + year() + "  " + hour() + ":" + minute() + ":" + second();
  
  textFont("Arial", 18);
  background(color(0,0,0));   // clears screen
  fill(color(255,255,255));   // set text fill color to white
  textAlign(LEFT);
  text(timestamp, 10, 20);    // display time on screen (top-left corner)
  
  textAlign(CENTER);
  text("Attempt " + (attempt + 1) + " out of 2 completed!", width/2, 60); 
  text("Hits: " + hits, width/2, 100);
  text("Misses: " + misses, width/2, 120);
  text("Accuracy: " + accuracy + "%", width/2, 140);
  text("Total time taken: " + test_time + "s", width/2, 160);
  text("Average time per target: " + time_per_target + "s", width/2, 180);
  text("Average time for each target (+ penalty): " + target_w_penalty + "s", width/2, 220);

  // Saves results (DO NOT CHANGE!)
  let attempt_data = 
  {
        project_from:       GROUP_NUMBER,
        assessed_by:        student_ID,
        test_completed_by:  timestamp,
        attempt:            attempt,
        hits:               hits,
        misses:             misses,
        accuracy:           accuracy,
        attempt_duration:   test_time,
        time_per_target:    time_per_target,
        target_w_penalty:   target_w_penalty,
  }
  
  // Sends data to DB (DO NOT CHANGE!)
  if (RECORD_TO_FIREBASE)
  {
    // Access the Firebase DB
    if (attempt === 0)
    {
      firebase.initializeApp(firebaseConfig);
      database = firebase.database();
    }
    
    // Adds user performance results
    let db_ref = database.ref('G' + GROUP_NUMBER);
    db_ref.push(attempt_data);
  }
}

// Mouse button was pressed - lets test to see if hit was in the correct target
function mousePressed() 
{
  
  // Only look for mouse releases during the actual test
  // (i.e., during target selections)
  if (draw_targets)
  {
    for (var i = 0; i < legendas.getRowCount(); i++)
    {
      // Check if the user clicked over one of the targets
      if (targets[i].clicked(mouseX, mouseY)) 
      {
        sound.play();
        // Checks if it was the correct target
        if (targets[i].id === trials[current_trial] + 1) {
          targets[i].clicked_right();
          hits++;
        } else {
          misses++;
        }
        
        current_trial++;              // Move on to the next trial/target
        break;
      }
    }
    
    // Check if the user has completed all trials
    if (current_trial === NUM_OF_TRIALS)
    {
      testEndTime = millis();
      draw_targets = false;          // Stop showing targets and the user performance results
      printAndSavePerformance();     // Print the user's results on-screen and send these to the DB
      attempt++;                      
      
      // If there's an attempt to go create a button to start this
      if (attempt < 2)
      {
        continue_button = createButton('START 2ND ATTEMPT');
        continue_button.mouseReleased(continueTest);
        continue_button.position(width/2 - continue_button.size().width/2, height/2 - continue_button.size().height/2);
      }
    }
    // Check if this was the first selection in an attempt
    else if (current_trial === 1) testStartTime = millis(); 
  }
}

// Evoked after the user starts its second (and last) attempt
function continueTest()
{
  for (let i = 0; i < targets.length; i++) {
    targets[i].unclick();
  }
  
  // Re-randomize the trial order
  randomizeTrials();
  
  // Resets performance variables
  hits = 0;
  misses = 0;
  
  current_trial = 0;
  continue_button.remove();
  
  // Shows the targets again
  draw_targets = true; 
}

// Creates and positions the UI targets
function createTargets(target_size, horizontal_gap, vertical_gap)
{
  let sortedTable = sortTableAlphabetically(legendas);
  //sortedTable = sortTableZigZag(sortedTable);
  // Define the margins between targets by dividing the white space 
  // for the number of targets minus one
  let v_margin = vertical_gap / (GRID_ROWS - 1);
  let h_margin = horizontal_gap / (GRID_COLUMNS -1);
 
  
  let column_counter = 0;
  let group_column_counter = 0;
  let group_row_counter = 0;
  
  let intragroup_column_counter = 0;
    
  // Vai sempre de 0 a 3, quando chega a 3 passamos para uma nova coluna no grupo
  let intragroup_row_counter = 0;
  
  // Starting x do grupo
  let base_x = h_margin + target_size;
  let base_y = v_margin + target_size;
  
  let group_x = base_x;
  let past_x = 0;
  let group_y = base_y;
  let past_greatest_y = 0;
  
  // fzr while nenhum x chega à width + target_size or smth faco isto qnd acabar o for paro o while se o while sair ent ajusto os valores e refaco (tenho outro loop)
  
  // Vamos iterar por todos os targets a criar
  for (var target_index = 0; target_index < 80; target_index++) {
    let target_id = sortedTable.getNum(target_index, 0);  
    let target_label = sortedTable.getString(target_index, 1);
    let target_first_letter = Array.from(target_label)[0];
    
    let past_label = "0";
    let past_first_letter = "0";
    let future_label = "0";
    let future_first_letter = "0";
    if (target_index > 0) {
      past_label = sortedTable.getString(target_index - 1, 1);
      past_first_letter = Array.from(past_label)[0];
    }
    if (target_index < 79) {
      future_label = sortedTable.getString(target_index + 1, 1);
      future_first_letter = Array.from(future_label)[0];
    }
    
    
    let target_x = group_x;
    let target_y = group_y;

    // Quando mudamos de grupo
    if (target_first_letter != past_first_letter && past_first_letter != "0") {
      // Vamos buscar o x do target anterior e adicionamos a margem entre grupos
      group_x = past_x + width/10;
      target_x = group_x;
      
      intragroup_row_counter = 0;
      intragroup_column_counter = 0;
      group_column_counter++;
      column_counter++;
    }
    
    // Se chegamos ao numero maximo de colunas por linha passamos para a linha de baixo
    // A coluna 11 é a 12ª coluna pois comecamos no 0
    if (column_counter == 11) {
      print("entrei ao menos");
      intragroup_column_counter = 0;
      group_column_counter = 0;
      column_counter = 0;
      intragroup_row_counter = 0;
      group_row_counter++;
      
      group_y = past_greatest_y + height/9;
      target_y = group_y;
      // Margem y entre grupos
//      target_y += group_row_counter * height/10;
      group_x = base_x;
      target_x = group_x;
    }
    
    // A cada 3 targets, passamos para a coluna do lado
    if (intragroup_row_counter == 3) {
      intragroup_row_counter = 0;
      intragroup_column_counter++;
      column_counter++; 
    }
    
    
    // Comecamos no starting x do grupo (group_x ja esta) e adicionamos a margem intragrupo
    target_x += intragroup_column_counter * width/14;
    
    
    // Margem y entre targets
    target_y += intragroup_row_counter * width/18;
    


    intragroup_row_counter++;
    
    past_x = target_x;
    if (target_y > past_greatest_y) {
      past_greatest_y = target_y;
    }
    
    
    let has_twin = 0;
    
    for (let j = 0; j < target_label.length; j++) {
      if (Array.from(target_label)[j] == Array.from(future_label)[j] && j > 1) {
        has_twin = j + 1;
        print("\n" + j);
        print(target_label + " " + future_label);
        print(Array.from(target_label)[j] + " " + Array.from(future_label)[j]);
      } else if (Array.from(target_label)[j] != Array.from(future_label)[j]) {
        break;
      }
    }
    
    // Se o has_twin nao vai fazer efeito entao vejo se é preciso comparar à palavra anterior instead
    if (has_twin < 3) {
      for (let j = 0; j < target_label.length; j++) {
      if (Array.from(target_label)[j] == Array.from(past_label)[j] && j > 1) {
        has_twin = j + 1;
        print("\n" + j);
        print(target_label + " " + past_label);
        print(Array.from(target_label)[j] + " " + Array.from(past_label)[j]);
      } else if (Array.from(target_label)[j] != Array.from(past_label)[j]) {
        break;
      }
    }
    }
    
    /*if (target_first_letter == future_first_letter && Array.from(target_label)[1] == Array.from(future_label)[1] && Array.from(target_label)[2] == Array.from(future_label)[2]) {
      has_twin = 1;
    }*/

    let target = new Target(target_x, target_y, target_size, target_label, target_id, has_twin);
    targets.push(target);
    
    print(target_first_letter + column_counter);
  }
  
  Target.prototype.assignColors(targets);
  
}
  
  /*
  // Draw by columns
  let group_x = 0;
  let groups_space_ocupy = 0;
  let group_y = 0.4;
  let group_size = 0;
  for (let index = 0; index < 80; index += group_size)
  {
    group_size = 0;
    let first_letter_group =  Array.from(sortedTable.getString(index, 1))[0];
    for (let see_ahead = index; see_ahead < 80; see_ahead++)  // Loop to check next group size
    {
      if (first_letter_group == Array.from(sortedTable.getString(see_ahead, 1))[0])
      {
        group_size += 1;
      }
      else
      {
        break;
      }
    }

    if (groups_space_ocupy + group_x > GRID_COLUMNS)
    {
      group_x = 0;
      groups_space_ocupy = 0;
      group_y += 1;
    }

    let individual_x = 0;
    let individual_y = 0;
    for (let target_index = index; target_index < index + group_size; target_index++)  // Assigned targets of said group
    {
      if (individual_y === 3)
      {
        individual_y = 0;
        individual_x += 1;
      }
      
      
      //group_x é a distancia que cada grupo tem entre si
      //group_space_ocupy balancea-se com group_x

      let target_x = 0.35*PPCM + (v_margin + target_size) * (individual_x * 0.02*PPCM + group_x * 0.01*PPCM + groups_space_ocupy* 0.02*PPCM) + target_size/2;        // give it some margin from the left border
      let target_y = (h_margin + target_size) * (individual_y * 0.018*PPCM + group_y * 0.057*PPCM) + target_size/2;      
      
      if (display_size < 15) {
        let target_x = 0.35*PPCM + (v_margin + target_size) * (individual_x * 0.025*PPCM + group_x * 0.015*PPCM + groups_space_ocupy* 0.02*PPCM) + target_size/2;        // give it some margin from the left border
        let target_y = (h_margin + target_size) * (individual_y * 0.022*PPCM + group_y * 0.06*PPCM) + target_size/2;        
      }

      //let target_x = 0.35*PPCM + (v_margin + target_size) * (individual_x * 0.015*PPCM + group_x * 0.01*PPCM + groups_space_ocupy* 0.014*PPCM) + target_size/2;        // give it some margin from the left border
      //let target_y = (h_margin + target_size) * (individual_y * 0.0155*PPCM + group_y * 0.057*PPCM) + target_size/2;
      print(PPCM);
      print(display_size);
      
//      let target_x = 20 + (v_margin + target_size) * (individual_x * 0.87 + group_x * 0.61 + groups_space_ocupy* 0.8) + target_size/2;        // give it some margin from the left border
      //let target_y = (h_margin + target_size) * (individual_y * 0.90 + group_y * 3.3) + target_size/2;

      individual_y += 1;

      let target_id = sortedTable.getNum(target_index, 0);  
      let target_label = sortedTable.getString(target_index, 1);   
      
      var has_twin = 0;
      
      let past_label = "0";
      let future_label = "0";
      
      if (target_index > 0) {
        past_label = sortedTable.getString(target_index - 1, 1);
      }
        
      if (target_index < 79) {
        future_label = sortedTable.getString(target_index + 1, 1);
      }
      
      for (var i = 0; i < target_label.length; i++) {
        if (i > 3) {
          has_twin = i;
        }
        if (Array.from(target_label)[i] == Array.from(future_label)[i] || Array.from(target_label)[i] == Array.from(past_label)[i]) {
          continue;
        } else {
          break;
        }
      }
       */ 
        /*if (Array.from(target_label)[0] == Array.from(future_label)[0] && Array.from(target_label)[1] == Array.from(future_label)[1] && Array.from(target_label)[2] == Array.from(future_label)[2]) {
          has_twin = 1;
        }*/
      /*
      let target = new Target(target_x + 40, target_y, target_size, target_label, target_id, has_twin);
      targets.push(target);
    }
    group_x += 1;
    groups_space_ocupy += ceil(group_size / 3);
  }
  
  
  
  Target.prototype.assignColors(targets);
  print(targets[0].assigned_color);
}
*/

// Is invoked when the canvas is resized (e.g., when we go fullscreen)
function windowResized() 
{
  if (fullscreen())
  {
    resizeCanvas(windowWidth, windowHeight);
    
    // DO NOT CHANGE THE NEXT THREE LINES!
    let display        = new Display({ diagonal: display_size }, window.screen);
    PPI                = display.ppi;                      // calculates pixels per inch
    PPCM               = PPI / 2.54;                       // calculates pixels per cm
  
    // Make your decisions in 'cm', so that targets have the same size for all participants
    // Below we find out out white space we can have between 2 cm targets
    let screen_width   = display.width * 2.54;             // screen width
    let screen_height  = display.height * 2.54;            // screen height
    let target_size    = 2;                                // sets the target size (will be converted to cm when passed to createTargets)
    let horizontal_gap = screen_width - target_size * GRID_COLUMNS;// empty space in cm across the x-axis (based on 10 targets per row)
    let vertical_gap   = screen_height - target_size * GRID_ROWS;  // empty space in cm across the y-axis (based on 8 targets per column)

    // Creates and positions the UI targets according to the white space defined above (in cm!)
    // 80 represent some margins around the display (e.g., for text)
    createTargets(target_size * PPCM, horizontal_gap * PPCM - 80, vertical_gap * PPCM - 80);

    // Starts drawing targets immediately after we go fullscreen
    draw_targets = true;
  }
}


function sortTableAlphabetically(table) {
  let rowsArray = []; // Temporary array to hold rows
  // Convert legendas table rows into an array
  for (let i = 0; i < table.getRowCount(); i++) {
    let id = table.getNum(i, 0);  // Get the first column (ID)
    let label = table.getString(i, 1);  // Get the second column (text label)
    rowsArray.push({ id, label }); // Store as an object
  }
  
  // Sort the array alphabetically based on `label`
  rowsArray.sort((a, b) => a.label.localeCompare(b.label));
  
  // Create a new table to store sorted values
  let sortedTable = new p5.Table();
  sortedTable.addColumn('id');
  sortedTable.addColumn('label');

  // Add sorted data back into the new table
  for (let row of rowsArray) {
    let newRow = sortedTable.addRow();
    newRow.setNum('id', row.id);
    newRow.setString('label', row.label);
  }

  // Replace legendas with the sorted table
  //legendas = sortedTable;
  
  return sortedTable;
  //saveTable(sortedTable, 'filename.csv', 'csv');
}







