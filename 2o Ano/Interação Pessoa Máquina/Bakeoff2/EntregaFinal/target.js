// Target class (position and width)
class Target
{
  constructor(x, y, w, l, id, has_twin)
  {
    this.x      = x;
    this.y      = y;
    this.width  = w;
    this.label  = l;
    this.id     = id;
    this.has_twin = has_twin;
    this.was_clicked = 0;
    this.draw_letter_label = 0;
    
    // Declare colors
    this.colors = [
      [29, 132, 103],  //Darker green
      [52, 160, 164],    //Greenish blue
      [22, 138, 173],  //Lighter blue
      [24, 78, 119]   //Darker blue
    ]
/*
    this.colors = [
      [162, 11, 59],  // Dark
      [241, 57, 103],  // 
      [235, 112, 130],  // 
      [245, 124, 164],  // 
      [248, 169, 191]    // Light
    ];
    */
    
    this.assigned_color = [0, 0, 0];
    
    //this.clicked_color = [118, 139, 219];
    this.clicked_color = [235, 112, 130];
  }
  
  // Checks if a mouse click took place
  // within the target
  clicked(mouse_x, mouse_y)
  {
    return dist(this.x, this.y, mouse_x, mouse_y) < this.width / 2;
  }

  clicked_right() {
    this.was_clicked = 1;
  }
  
  unclick() {
    this.was_clicked = 0;
  }

  
  // Draws the target (i.e., a circle)
  // and its label
  draw()
  {
    
    // Draw target 
    if (this.was_clicked == 1) {
      fill(this.clicked_color);
    } else {
      fill(this.assigned_color);
    }
    
    noStroke();
    rect(this.x - (this.width*0.92) / 2, this.y - (this.width*0.92) / 2, this.width*0.92, this.width*0.72);

    
  if (this.draw_letter_label == 1) {
    // Draw label
    noStroke();
    textFont("Arial", 35);
    fill(this.assigned_color);
    textAlign(CENTER);
    text(this.label[0], this.x - this.width*0.68, this.y - this.width*0.22);
  }

    
    // Draw label
    textFont("Arial", 15);
    /*
    if(this.assigned_color == this.colors[3] || this.assigned_color == this.colors[4]) {
      fill(color(0,0,0));
    } else {
      fill(color(255,255,255));
    }
    */
    fill(color(255,255,255));
    textAlign(CENTER);
    text(this.label, this.x, this.y - this.width*0.17);
    
    textFont("Arial", 30);
    /*
    if(this.assigned_color == this.colors[3] || this.assigned_color == this.colors[4]) {
      fill(color(0,0,0));
    } else {
      fill(color(255,255,255));
    }
    */
    fill(color(255,255,255));
    textAlign(CENTER);
    
    var first_chars = '';


    first_chars = Array.from(this.label)[0] + Array.from(this.label)[1] + Array.from(this.label)[2];
    
    if (this.has_twin > 2) {
      for (var i = 3; i < this.has_twin + 1; i++) {
        first_chars += Array.from(this.label)[i];
      }
    }
    
    text(first_chars, this.x, this.y + 10);
  }
  /*
  assignColors(targets) {
    let first_letter = "";
    let draw_label = 0;
    
    for (let i = 0; i < targets.length; i++) {
      first_letter = targets[i].label[0];
      
      switch (first_letter) {
        case "A":
          targets[i].assigned_color = [24, 78, 119];    // Dark blue
          break;
        case "B":
          targets[i].assigned_color = [22, 138, 173];
          break;  
        case "C":
          targets[i].assigned_color = [52, 160, 164];
          break;
        case "D":
          targets[i].assigned_color = [235, 112, 130];    // Light pink
          break;
        case "G":
          targets[i].assigned_color = [241, 57, 103];
          break;
        case "H":
          targets[i].assigned_color = [162, 11, 59];      // Dark pink
          break;
        case "I":
          targets[i].assigned_color = [162, 11, 59];      // Dark pink
          break;
        case "J":
          targets[i].assigned_color = [241, 57, 103];
          break;
        case "K":
          targets[i].assigned_color = [235, 112, 130];
          break;
        case "L":
          targets[i].assigned_color =  [248, 96, 146];
          break;
        case "M":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "N":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "O":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "P":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "Q":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "R":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "S":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "T":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "V":
          targets[i].assigned_color = [255, 255, 255];
          break;
        case "W":
          targets[i].assigned_color = [255, 255, 255];
          break;
        default:
          targets[i].assigned_color = [255, 255, 255];
          
      }
    }
  }
  */
  
  
  // Draws the circle buttons
  
  assignColors(targets) {
    let first_letter = "";
    let color_index = -1;
    let draw_label = 0;

    for (let i = 0; i < targets.length; i++) {
      


      if (targets[i].label[0] !== first_letter)  // Compare first letter
      {
        // Change color
        color_index += 1;
        color_index = color_index % targets[i].colors.length;  // This ensures we start from the beginning once we exceed the max

        targets[i].draw_letter_label = 1;
      }
      targets[i].assigned_color = targets[i].colors[color_index];
      //fill(targets[i].colors[color_index]); 
    
      /*
      if (targets[i].was_clicked == 1) 
      {
        fill(targets[i].clicked_color);
      }
      */
      
      /*
      // Draw target 
      noStroke();
      rect(targets[i].x - (targets[i].width*0.92) / 2, targets[i].y - (targets[i].width*0.92) / 2, targets[i].width*0.92, targets[i].width*0.72);

      if (draw_label === 1)
      {
        // Draw label
        noStroke();
        textFont("Arial", 35);
        fill(targets[i].colors[color_index]);
        textAlign(CENTER);
        text(targets[i].label[0], targets[i].x - 155, targets[i].y - 15);
        draw_label = 0;
      }
      */
  
      first_letter = targets[i].label[0];
    }
  }
  
}


