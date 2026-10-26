const {test}=require('node:test');
const assert=require('node:assert/strict');
const {RightDrag}=require('./right_drag_policy.cjs');
test('stationary right click is deferred until release',()=>{
 const s=new RightDrag();
 assert.deepEqual(s.sample(true,10,10),{orbit:false,click:false});
 assert.deepEqual(s.sample(false,10,10),{orbit:false,click:true});
 assert.equal(s.sample(false,10,10).click,false);
});
test('drag orbits after six pixels and never casts on release',()=>{
 const s=new RightDrag();s.sample(true,0,0);
 assert.equal(s.sample(true,7,0).orbit,true);
 assert.equal(s.sample(false,7,0).click,false);
});
test('return to origin remains a drag',()=>{
 const s=new RightDrag();s.sample(true,0,0);s.sample(true,20,0);
 assert.equal(s.sample(true,0,0).orbit,true);
 assert.equal(s.sample(false,0,0).click,false);
});
test('UI press and loss of focus cancel until release',()=>{
 for(const focus of [true,false]){
  const s=new RightDrag();s.sample(true,0,0,focus,false);
  assert.equal(s.sample(true,20,0).orbit,false);
  assert.equal(s.sample(false,20,0).click,false);
 }
 const s=new RightDrag();s.sample(true,0,0);s.sample(true,10,0,false);
 assert.equal(s.sample(false,10,0).click,false);
});
test('next gesture works after cancellation',()=>{
 const s=new RightDrag();s.sample(true,0,0,false);s.sample(false,0,0);
 s.sample(true,0,0);assert.equal(s.sample(true,0,7).orbit,true);
});
