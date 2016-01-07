
function tstparam(){

    function param1(){
        //param3(Array.prototype.slice.call(arguments),"p from f1");
        //param3(arguments,"p from f1");
        //param3.apply(this,arguments,"p from f1");
        //param3.apply(this,[111,222],"p from f1");

        var args=Array.prototype.slice.call(arguments);
        args.splice(0,0,"p from f1"); //insert a param to the head
        param3.apply(this,args);
        param3.apply(param3,args);
    }

    function param2(p1,p2){
        console.log("param2:p1="+p1+",p2="+p2);
    }

    function param3(p1,p2,p3){
        console.log("param3:p1="+p1+",p2="+p2+",p3="+p3);
    }

    param1(1,"2");

    //param2(1,2,3);

}

tstparam();

