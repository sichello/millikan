void hist_2();
float get_mean(float values[], int entries[]);

int main()
{
  hist();
  return 0;
}

void hist_2()
{
#include <iostream>
  const float eknown = 1.602*pow(10,-19);

  ifstream infile;
  infile.open ("data/q_spreadsheet.csv");
  if(!infile)
    {
      cout << "Error opening file" << endl;
      return 1;
    }

  int lines = 0;
  string line;
  while(!infile.eof())
    {
      getline(infile,line);
      lines++;
    }
  
  entries = lines - 11;

  infile.clear();              // forget we hit the end of file
  infile.seekg(0, ios::beg);   // move to the start of the file
  
  for (int j=0; j<=10;j++)
    getline(infile,line); //skip header
 
 
  TH1F *h1 = new TH1F("h1", "Electron Charge Measurements", 75, 1*10^(-19),7*10^(-19));
  h1->SetYTitle("Frequency");
  h1->SetXTitle("Charge (Coulumbs)");
  
  float z[50], ez[50];
  float div_q;
  string dummy;
  int current_drop=1;
  int next_drop=0;
  int trial_no=0;
  float drop_mean=0;
  float drop_sterr=0; 
  infile >> dummy >> current_drop;
  int i = 0;
  while (i<(entries) && !infile.eof()) //change to i<entries
    {
      for(int j=0; j<=13; j++) //change j<=13
	infile >> line;
      
      infile >> z[trial_no];
      infile >> dummy;
      infile >> dummy >> next_drop;
      
      // cout << " Drop No = " << current_drop << endl;
      //cout << " Q Value = " << z[trial_no] << endl;
      ez[trial_no] = 0;
      
      if (current_drop != next_drop || i==(entries-2)  )
	{
	  drop_mean = get_mean(z,trial_no+1);
	  drop_sterr = get_sterr(z,trial_no+1);
	  h1->Fill(drop_mean);
	  div_q = drop_mean/eknown;
	 
	  cout << endl;
	  cout << "***************************" << endl;
	  cout << "Drop Number  = " << current_drop << endl;
	  cout << "Trials       = " << trial_no+1 << endl;
	  cout << "Mean Q Value = " << drop_mean << endl;
	  cout << "Q/e          = " << div_q << endl;
	  cout << "***************************" << endl; 
	  
	  trial_no=0;
	  current_drop=next_drop;
	}
      else
	trial_no++;

      i++;
    }
    

  cout << "There are " << entries << " entries in the data set." << endl;
  infile.close();

  TCanvas *c1 = new TCanvas("c1","Length",100,10,1000,500);
  c1->SetGrid();
  h1->SetLabelSize(.03);
  h1->SetFillColor(3);
  gStyle->SetOptFit(2200);
  gStyle->SetOptStat("ne");

  h1->Draw();
  
    
  
  float min = 0.2*eknown;
  float max = 4.8*eknown;
  
  TF1 *gfit = new TF1("Gaussians","[1]*exp(-0.5*(x-[0])^2/[2]^2) + [3]*exp(-0.5*(x-2.*[0])^2/[4]^2) + [5]*exp(-0.5*(x-3.*[0])^2/[6]^2) + [7]*exp(-0.5*(x-4.*[0])^2/[8]^2)",min,max); // Create the fit function
  gfit->SetNpx(10000);
  gfit->SetLineWidth(2);
  
  gfit->SetParLimits(0,0.8*eknown,1.2*eknown);  

  gfit->SetParameter(0,1.6E-19);
  gfit->SetParLimits(1,3,25);
  gfit->SetParLimits(2,1.E-21,eknown/2.);
  gfit->SetParameter(2,1.E-20);

  gfit->SetParLimits(3,3,25);
  gfit->SetParameter(3,10);
  gfit->SetParLimits(4,1.E-21,eknown/2.);
  gfit->SetParameter(4,1.E-20);

  gfit->SetParLimits(5,3,25);
  gfit->SetParameter(5,10);
  gfit->SetParLimits(6,1.E-21,eknown/2.);
  gfit->SetParameter(6,1.E-20);

  gfit->SetParLimits(7,3,25);
  gfit->SetParameter(7,10);  
  gfit->SetParLimits(8,1.E-21,eknown/2.);
  gfit->SetParameter(8,1.E-20); 

  h1->Fit(gfit,"MIER");
  h1->SetLineWidth(0.01);
  gfit->Draw("same");

  float e_val = gfit->GetParameter(0);
  float ee_val = gfit->GetParError(0);

  TPaveText *pt1 = new TPaveText(0,0,.3,.2,"NDC");
  char str[50];
  sprintf(str, "e = (%3.3f #pm %3.3f)x10^{-19}",e_val/pow(10,-19),ee_val/pow(10,-19));
  pt1->AddText(str);
  pt1->Draw("same");
  c1->Update();
}

///////////////////////////////////////////////////////////////////////
float get_mean(float values[], int entries)
{
  float ave=0;
  float sum=0;
  for(int j=0; j<entries; j++)
    sum = sum + values[j];
    
  ave = sum/entries; 
  return ave;
}
///////////////////////////////////////////////////////////////////////
float get_stdev(float values[], int entries)
{  
  float ave=0, sum=0, diff=0, stsum=0, stdev=0;
  for(int j=0; j<entries; j++)
    sum = sum + values[j];
    
  ave = sum/entries; 
  
  stsum = 0;
  for(int j=0; j<entries; j++)
    {
      diff = ave - values[j];
      stsum = stsum + pow(diff,2);
    }
  stdev = sqrt(stsum/(entries));
  return stdev;
}
////////////////////////////////////////////////////////////////////////
float get_sterr(float values[], int entries)
{
  float ave=0, sum=0, diff=0, stsum=0, stdev=0, sterr=0;
  for(int j=0; j<entries; j++)
    sum = sum + values[j];
    
  ave = sum/entries; 
  
  stsum = 0;
  for(int j=0; j<entries; j++)
    {
      diff = ave - values[j];
      stsum = stsum + pow(diff,2);
    }
  return sterr;
}
////////////////////////////////////////////////////////////////////////
void save_data_file()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [50];
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  strftime (buffer,40,"%b%d_%Hh.%Mm.%Ss.txt",timeinfo);
  string opfname = buffer;
  cout << opfname << endl;
  string mkfile = "cp temp.txt data/"+opfname;
  system(mkfile.c_str());
  return;
}

/////////////////////////////////////////////////////////////////////////
std::string get_date(void)
{
  time_t now;
  char the_date[MAX_DATE];

  the_date[0] = '\0';

  now = time(NULL);

  if (now != -1)
    {
      strftime(the_date, MAX_DATE, "%d_%m_%Y", gmtime(&now));
    }

  return std::string(the_date);
}

