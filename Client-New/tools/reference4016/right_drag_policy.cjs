// QA gesture arbitration. Native skill clicks are delivered on release; drags consume them.
class RightDrag {
 constructor(){this.held=false;this.drag=false;this.cancel=false;}
 sample(held,x,y,focused=true,allowed=true){
  let click=false;
  if(held&&!this.held){this.x=x;this.y=y;this.drag=false;this.cancel=!focused||!allowed;}
  if(!focused)this.cancel=true;
  if(held&&!this.cancel&&(x-this.x)**2+(y-this.y)**2>36)this.drag=true;
  if(!held&&this.held)click=!this.cancel&&!this.drag;
  this.held=held;
  return {orbit:held&&this.drag&&!this.cancel,click};
 }
}
if(typeof module!=='undefined')module.exports={RightDrag};
