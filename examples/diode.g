# tdfc-gappa backend autocompiled body file
# tdfc version 1.160
# Tue Aug  2 23:02:33 2011


@fx = fixed<-32,ne>;

vj_m = float<ieee_64,ne>(0.0258642);
isat_m = float<ieee_64,ne>(0.001);
v_m = float<ieee_64,ne>(v);
diode::diode(double n_vj,double n_isat,DOUBLE_SCORE_STREAM n_v,DOUBLE_SCORE_STREAM n_i,DOUBLE_SCORE_STREAM n_gm)
{
  int *params=(int *)malloc(2*sizeof(int));
  vj=n_vj;
  params[0]=n_vj;
  isat=n_isat;
  params[1]=n_isat;
  addInstance(instances,params);
  char * name=mangle(diode_name,2,params);
  char * instance_fn=resolve(name);
  assert (instance_fn==(char *)NULL);
    declareIO(1,2, diode_name);
    bindInput(0,n_v,new ScoreStreamType(0,0,1,64));
    SCORE_MARKREADSTREAM(n_v,globalCounter->threadCounter);
    bindOutput(0,n_i,new ScoreStreamType(0,0,1,64));
    SCORE_MARKWRITESTREAM(n_i,globalCounter->threadCounter);
    bindOutput(1,n_gm,new ScoreStreamType(0,0,1,64));
    SCORE_MARKWRITESTREAM(n_gm,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&diode_proc_run, this);
}
divide_8 = ( v / à˜•de_8 );
