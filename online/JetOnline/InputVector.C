#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

#include "rhicpol.h"
#include "rpoldata.h"
#include "rhic_offline.h"
#include "rhic_class.h"
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

#include "input_vector.h"

int TAsym::InputVector(processWEvent *wevent,int *total_event,int all_event){
        int k,j;
	int search_i;
	int current_event,current_hit;
        int test2=0;

        int check_rev=-1;
        int check_bu=-110;

        current_event=v_event.size();
//        printf("now is Event\n");
        if(wevent->revolution==check_rev && wevent->bunch==check_bu){
          printf("CHECK rev=%d bu=%d\n",wevent->revolution,wevent->bunch);
       	  printf("hjet_count=%d rev=%d bunch=%d all_event=%d ch=%d total_event=%d current_event=%d\n",
          wevent->hjet_count,wevent->revolution,wevent->bunch,all_event,wevent->geo,*total_event,current_event);
		getchar();
        }
	if(*total_event==-1){
//ROOT Tree Output//////////////////
            current_event=v_event.size();
            printf("ROOT TREE current_event=%d\n",current_event);
            if(current_event>2){
/*
                printf("InputVector status check!!---------------------\n");		
                current_event=v_event.size();
                for(j=0;j<current_event;++j){
                     printf("current_event=%d v_event[%d].rev=%d v_event[%d].hjet_count=%d\n",
                       current_event,j,v_event[j].rev,j,v_event[j].hjet_count);
                     if(j%100==0 && j>0)getchar();
                }
                printf("status_END---------------------------------------\n");
                getchar();
*/              
               for(j=0;j<current_event;++j){
                  TreeJet.event_num=v_event[j].event_num;
                  TreeJet.revolution=v_event[j].rev;
                  TreeJet.bunch=v_event[j].bunch;
	          TreeJet.hit_num=v_event[j].hit.size();
                  TreeJet.hjet_count=v_event[j].hjet_count;
                  TreeJet.target_inf=v_event[j].target_inf;
                  TreeJet.array_num=17*(TreeJet.hit_num);
/*
                  printf("TREE WRITE current_event=%d hjet_count=%d rev=%d bunch=%d hit_num=%d\n",current_event,TreeJet.hjet_count,TreeJet.revolution,
                      TreeJet.bunch,TreeJet.hit_num);
                  if(j%10==0 && j>0)getchar();
*/
                  for(k=0;k<(TreeJet.hit_num);++k){
                     TreeJet.HitData[k*17+0]=(double)v_event[j].hit[k].strip;       
                     TreeJet.HitData[k*17+1]=(double)v_event[j].hit[k].amp;       
                     TreeJet.HitData[k*17+2]=(double)v_event[j].hit[k].time;       
                     TreeJet.HitData[k*17+3]=(double)v_event[j].hit[k].tmax;       
                     TreeJet.HitData[k*17+4]=(double)v_event[j].hit[k].intg;       
                     TreeJet.HitData[k*17+5]=(double)v_event[j].hit[k].amp2;       
                     TreeJet.HitData[k*17+6]=(double)v_event[j].hit[k].time2;       
                     TreeJet.HitData[k*17+7]=(double)v_event[j].hit[k].tmax2;       
                     TreeJet.HitData[k*17+8]=(double)v_event[j].hit[k].intg2;       
                     TreeJet.HitData[k*17+9]=(double)v_event[j].hit[k].amp3;       
                     TreeJet.HitData[k*17+10]=(double)v_event[j].hit[k].time3;       
                     TreeJet.HitData[k*17+11]=(double)v_event[j].hit[k].tmax3;       
                     TreeJet.HitData[k*17+12]=(double)v_event[j].hit[k].intg3;       
                     TreeJet.HitData[k*17+13]=(double)v_event[j].hit[k].f_amp;       
                     TreeJet.HitData[k*17+14]=(double)v_event[j].hit[k].f_time;       
                     TreeJet.HitData[k*17+15]=(double)v_event[j].hit[k].f_tmax;       
                     TreeJet.HitData[k*17+16]=(double)v_event[j].hit[k].f_intg;       
                  //printf("%lf %lf %lf %lf %lf %lf %lf\n",TreeJet.HitData[k*17+0],TreeJet.HitData[k*17+2],TreeJet.HitData[k*17+3],
                  //             TreeJet.HitData[k*17+6],TreeJet.HitData[k*17+10],TreeJet.HitData[k*17+13],TreeJet.HitData[k*17+15]);
                  //getchar();
                  }
                  JetEvent->Fill();
               }
               JetEvent->Write();
            
               printf("Write Tree -> EVENT %d\n",current_event);
               //getchar();
               for(j=0;j<current_event;++j){
                   v_event[j].hit.clear();       
               }
               v_event.clear();
               current_event=v_event.size();
               printf("AFTER CLEAR current_event=%d\n",current_event);
               //getchar();
            }//if current_event>2
            if(wevent->revolution != 0 && wevent->bunch !=0 ){
                test_event.event_num=0;
	        test_event.rev=0;
	        test_event.bunch=0;
	        test_event.hit_num=0;
	        test_event.hjet_count=0;
	        test_event.target_inf=0;
                v_event.push_back(test_event);
            }
            test_event.event_num=(wevent->revolution)*bunch_typ+(wevent->bunch);
	    test_event.rev=wevent->revolution;
	    test_event.bunch=wevent->bunch;
	    test_event.hit_num=1;
	    test_event.hjet_count=wevent->hjet_count;
	    test_event.target_inf=wevent->target_inf;
            v_event.push_back(test_event);
		  
            hit_test.amp=wevent->amp;
	    hit_test.time=wevent->t_time;
	    hit_test.tmax=wevent->tmax;
	    hit_test.intg=wevent->intg;
            hit_test.amp2=wevent->amp2;
	    hit_test.time2=wevent->t_time2;
	    hit_test.tmax2=wevent->tmax2;
	    hit_test.intg2=wevent->square2;
            hit_test.amp3=wevent->amp3;
	    hit_test.time3=wevent->t_time3;
	    hit_test.tmax3=wevent->tmax3;
	    hit_test.intg3=all_event;
	    //hit_test.intg3=wevent->square3;
            hit_test.f_amp=wevent->f_amp;
	    hit_test.f_time=wevent->f_t_time;
	    hit_test.f_tmax=wevent->f_tmax;
	    hit_test.f_intg=wevent->f_square;
	    hit_test.strip=wevent->geo;
	    current_event=v_event.size();
            v_event[current_event-1].hit.push_back(hit_test);
            current_hit=v_event[current_event-1].hit.size();
	    *total_event=current_event-1;
            //printf("HAJIME\n");
            //printf("v_event[0].bunch=%d v_event[1].bunch=%d\n",v_event[0].bunch,v_event[1].bunch);
            //getchar();
///////////////////////////////////////checkEND
	}else{
            for(search_i=0;search_i<(v_event.size());++search_i){
		if((wevent->revolution)==v_event[search_i].rev ){
                    if( (wevent->bunch)==v_event[search_i].bunch){
		        //printf("find! \n");getchar();
			hit_test.amp=wevent->amp;
			hit_test.time=wevent->t_time;
		        hit_test.tmax=wevent->tmax;
                        hit_test.amp2=wevent->amp2;
	                hit_test.time2=wevent->t_time2;
	                hit_test.tmax2=wevent->tmax2;
	                hit_test.intg2=wevent->square2;
                        hit_test.amp3=wevent->amp3;
	                hit_test.time3=wevent->t_time3;
	                hit_test.tmax3=wevent->tmax3;
	                hit_test.intg3=all_event;
	                //hit_test.intg3=wevent->square3;
                        hit_test.f_amp=wevent->f_amp;
	                hit_test.f_time=wevent->f_t_time;
	                hit_test.f_tmax=wevent->f_tmax;
	                hit_test.f_intg=wevent->f_square;
			hit_test.strip=wevent->geo;
			current_event=search_i;
//		        printf("current_event=%d\n",current_event);
			v_event[search_i].hit.push_back(hit_test);
			current_hit=v_event[search_i].hit.size();
//            	        printf("current_event=%d current_hit=%d\n",current_event,current_hit);
			break;
                    }
                    if( (wevent->bunch)<v_event[search_i].bunch || v_event[search_i+1].rev > (wevent->revolution)){
                        int insert_typ;
        if(wevent->revolution==check_rev && wevent->bunch==check_bu){
          printf("CHECK rev=%d bu=%d\n",wevent->revolution,wevent->bunch);
       	  printf("hjet_count=%d rev=%d bunch=%d all_event=%d ch=%d total_event=%d current_event=%d\n",
          wevent->hjet_count,wevent->revolution,wevent->bunch,all_event,wevent->intg,*total_event,current_event);
		getchar();
        }
                        //printf("INSERT BUNCH!! wevent->bunch=%d  v_event[%d].bunch=%d v_event[%d].bunch=%d\n",
                        //       wevent->bunch,search_i,v_event[search_i].bunch,search_i-1,v_event[search_i-1].bunch);
                        vector<eventData>::iterator where = v_event.begin();
                        if( (wevent->bunch)<v_event[search_i].bunch){
                            where +=(search_i);
                            insert_typ=0;
                        }else{
                            where +=(search_i+1);
                            insert_typ=1;
                        }
			test_event.event_num=wevent->revolution*bunch_typ+wevent->bunch;
			test_event.rev=wevent->revolution;
	                test_event.bunch=wevent->bunch;
		        test_event.hit_num=1;
	                test_event.hjet_count=wevent->hjet_count;
	                test_event.target_inf=wevent->target_inf;
                        v_event.insert(where,test_event);
			hit_test.amp=wevent->amp;
			hit_test.time=wevent->t_time;
		        hit_test.tmax=wevent->tmax;
                        hit_test.amp2=wevent->amp2;
	                hit_test.time2=wevent->t_time2;
	                hit_test.tmax2=wevent->tmax2;
	                hit_test.intg2=wevent->square2;
                        hit_test.amp3=wevent->amp3;
	                hit_test.time3=wevent->t_time3;
	                hit_test.tmax3=wevent->tmax3;
	                //hit_test.intg3=wevent->square3;
	                hit_test.intg3=all_event;
                        hit_test.f_amp=wevent->f_amp;
	                hit_test.f_time=wevent->f_t_time;
	                hit_test.f_tmax=wevent->f_tmax;
	                hit_test.f_intg=wevent->f_square;
			hit_test.strip=wevent->geo;
		        current_event=v_event.size();
                        if(insert_typ==0)v_event[search_i].hit.push_back(hit_test);
                        if(insert_typ==1)v_event[search_i+1].hit.push_back(hit_test);
            if(check_rev==wevent->revolution && check_bu==wevent->bunch){
                 if(search_i>0){
                        printf("INSERT!! typ=%d search=%d wevent->bunch=%d  v_event[%d].bunch=%d v_event[%d].bunch=%d v_event[%d].bunch=%d\n",
                               insert_typ,search_i,wevent->bunch,search_i+1,v_event[search_i+1].bunch,search_i,v_event[search_i].bunch,search_i-1,v_event[search_i-1].bunch);
                 }else if(search_i==0){
                        printf("INSERT!! typ=%d search=%d wevent->bunch=%d  v_event[%d].bunch=%d v_event[%d].bunch=%d \n",
                               insert_typ,search_i,wevent->bunch,search_i+1,v_event[search_i+1].bunch,search_i,v_event[search_i].bunch);
                 }
                        getchar(); 
            }
                        break;
                    }
                }
                if( (wevent->revolution)<v_event[search_i].rev && (wevent->revolution)>v_event[search_i-1].rev){
                   // printf("INSERT REVOLUTION!! wevent->revolution=%d  v_event[%d].rev=%d v_event[%d].rev=%d\n",
                   //       wevent->revolution,search_i,v_event[search_i].rev,search_i-1,v_event[search_i-1].rev);
                    vector<eventData>::iterator where = v_event.begin();
                    where +=(search_i);
		    test_event.event_num=wevent->revolution*bunch_typ+wevent->bunch;
		    test_event.rev=wevent->revolution;
	            test_event.bunch=wevent->bunch;
		    test_event.hit_num=1;
	            test_event.hjet_count=wevent->hjet_count;
	            test_event.target_inf=wevent->target_inf;
                    v_event.insert(where,test_event);
		    hit_test.amp=wevent->amp;
		    hit_test.time=wevent->t_time;
		    hit_test.tmax=wevent->tmax;
                    hit_test.amp2=wevent->amp2;
	            hit_test.time2=wevent->t_time2;
	            hit_test.tmax2=wevent->tmax2;
	            hit_test.intg2=wevent->square2;
                    hit_test.amp3=wevent->amp3;
	            hit_test.time3=wevent->t_time3;
	            hit_test.tmax3=wevent->tmax3;
	            //hit_test.intg3=wevent->square3;
	            hit_test.intg3=all_event;
                    hit_test.f_amp=wevent->f_amp;
	            hit_test.f_time=wevent->f_t_time;
	            hit_test.f_tmax=wevent->f_tmax;
	            hit_test.f_intg=wevent->f_square;
		    hit_test.strip=wevent->geo;
		    current_event=v_event.size();
                    v_event[search_i].hit.push_back(hit_test);
                   /// printf("INSERT!! wevent->revolution=%d  v_event[%d].rev=%d v_event[%d].rev=%d v_event[%d].rev=%d\n",
                   ///   wevent->revolution,search_i+1,v_event[search_i+1].rev,search_i,v_event[search_i].rev,search_i-1,v_event[search_i-1].rev);
                        
                    //getchar(); 
                    break;
                }
		if(search_i==(v_event.size()-1)){
                    //if(v_event.size()>300){
	//		printf("ppp not fined\n");
        //                int tmp_size=v_event.size();
        //                for(int kkk=0;kkk<tmp_size;++kkk){
        //                    printf("current_event=%d  v_event[%d].rev=%d v_event[%d].bunch=%d wevent->revolution=%d wevent->bunch=%d\n",
        //                      tmp_size,kkk,v_event[kkk].rev,kkk,v_event[kkk].bunch,wevent->revolution,wevent->bunch);
        //                    if(kkk%10==0)getchar();
        //                }
        //             }
			test_event.event_num=wevent->revolution*bunch_typ+wevent->bunch;
			test_event.rev=wevent->revolution;
	                test_event.bunch=wevent->bunch;
		        test_event.hit_num=1;
	                test_event.hjet_count=wevent->hjet_count;
	                test_event.target_inf=wevent->target_inf;
                        v_event.push_back(test_event);
		  
                        hit_test.amp=wevent->amp;
		        hit_test.time=wevent->t_time;
		        hit_test.tmax=wevent->tmax;
                        hit_test.amp2=wevent->amp2;
	                hit_test.time2=wevent->t_time2;
	                hit_test.tmax2=wevent->tmax2;
	                hit_test.intg2=wevent->square2;
                        hit_test.amp3=wevent->amp3;
	                hit_test.time3=wevent->t_time3;
	                hit_test.tmax3=wevent->tmax3;
	                //hit_test.intg3=wevent->square3;
	                hit_test.intg3=all_event;
                        hit_test.f_amp=wevent->f_amp;
	                hit_test.f_time=wevent->f_t_time;
	                hit_test.f_tmax=wevent->f_tmax;
	                hit_test.f_intg=wevent->f_square;
		        hit_test.strip=wevent->geo;
		        current_event=v_event.size();
                        v_event[current_event-1].hit.push_back(hit_test);
                        current_hit=v_event[current_event-1].hit.size();
//		        printf("current_event=%d current_hit=%d\n",current_event,current_hit);getchar();
			break;
		}
	    }
	}
/*
        printf("InputVector status check!!---------------------\n");		
        current_event=v_event.size();
        for(j=0;j<current_event;++j){
             printf("current_event=%d v_event[%d].rev=%d v_event[%d].hjet_count=%d\n",
               current_event,j,v_event[j].rev,j,v_event[j].hjet_count);

           if(v_event[j+1].rev-v_event[j].rev>1){
             printf("current_event=%d w_event[%d].rev=%d w_event[%d].rev=%d\n",current_event,j+1,v_event[j+1].rev,j,v_event[j].rev);
           //  getchar();
           }

        } 
        printf("status_END---------------------------------------\n");
        getchar();
*/
	return 0;
}
