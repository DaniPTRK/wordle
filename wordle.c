//Ion Daniel 315CC
#include<ncurses.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

void initialize_game(WINDOW *win, int l, int c, int **poz)
{
	//Se prepară ecranul de joc.
	int i, ni=0, j;
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	wbkgd(win, COLOR_PAIR(1));
	wattron(win, A_BOLD);
	mvwprintw(win, (l/2-6)/2, (c-strlen("Wordle"))/2,"Wordle");
	wattroff(win, A_BOLD);
	//Pregătire tabla de joc.
	for(i=l/2-10; i<=l/2+10; i=i+4)
	{
		for(j=c/2-9; j<=c/2+7; j=j+4)
		{
			mvwhline(win,i,j,'-',2);
			mvwhline(win,i+2,j,'-',2);
			mvwvline(win,i,j,'|',2);
			mvwvline(win,i,j+2,'|',2);
			mvwaddch(win,i,j,'+');
			mvwaddch(win,i+2,j,'+');
			mvwaddch(win,i,j+2,'+');
			mvwaddch(win,i+2,j+2,'+');
			poz[ni][0]=i+1;
			poz[ni][1]=j+1;
			ni++;
		}
	}
	poz[30][0]=0;
	poz[30][1]=0;
	wrefresh(win);
}

void reverse_it(WINDOW *win, int l, int c, char **choice, int high, int sit)
{
	/* Opțiunea aleasă în meniu primește un highlight.
	 * Automat, opțiunea aleasă inițial este prima opțiune din listă.
	 * În funcție de sit, opțiunile se afișează deasupra titlului jocului sau pe mijlocul
	 * ecranului.
	 */
	int i;
	for(i=1-sit; i<3; i++)
	{
		if(i==high)
		{
			wattron(win, A_REVERSE);
			mvwprintw(win, (1-sit)*(l/2-6)/2+2*i+2, (c-strlen(choice[i]))/2, "%s", choice[i]);
			wattroff(win, A_REVERSE);
		}
		else
		{
			mvwprintw(win, (1-sit)*(l/2-6)/2+2*i+2, (c-strlen(choice[i]))/2, "%s", choice[i]);
		}
	}
	wrefresh(win);
}

int meniu_control(WINDOW *win, int l, int c, int **poz, int sit)
{
	/*Aici este implementat meniul de control. choice[] ține minte alegerile pe care le poate
	 * realiza utilizatorul, iar acestea sunt afișate una sub alta.
	 * Utilizatorul alege dând click de 2 ori (excepție dacă vrea din start un nou joc) dacă
	 * vrea joc nou sau vrea să iasă.
	 * Dacă sit este 1, atunci voi afișa și butonul de continuare a jocului (meniul este chemat
	 * în timpul jocului).
	 * Dacă sit este 0, atunci afișez doar ultimele 2 opțiuni (meniul este chemat la final de joc).
	*/
    	int i, ch, lastch=1-sit;
	MEVENT event;
	char *choice[]={"Continua joc", "Incepe un nou joc", "Iesire joc"};
    	if(sit==0)
	{
		for(i=0; i<=l; i++)
    		{
			wmove(win,i,0);
        		wclrtoeol(win);
    		}
	}
	wattron(win, A_BOLD);
	mvwprintw(win, (1-sit)*(l/2-6)/2, (c-strlen("Meniu"))/2, "Meniu");
	wattroff(win, A_BOLD);
	reverse_it(win,l,c,choice,lastch,sit);
	mousemask(ALL_MOUSE_EVENTS,NULL);
	//Aștept click-ul și verific alegerea.
	while(1)
	{
		ch=wgetch(win);
		if((ch==KEY_MOUSE)&&(getmouse(&event)==OK)&&((event.bstate&BUTTON1_CLICKED)!=0))
		{
			for(i=1-sit; i<3; i++)
			{
				if((event.y==(1-sit)*(l/2-6)/2+2*i+2)&&(event.x>=(1-sit)*(c-strlen(choice[i]))/2)&&(event.x<=(1-sit)*(c+strlen(choice[i]))/2-1))
				{
					if(lastch==i)
					{
						//Returnăm opțiunea aleasă.
						return i;
					}
					else
					{
						//Highlight noii opțiuni.
						lastch=i;
						reverse_it(win,l,c,choice,lastch,sit);
					}
				}
			}
		}
	}
}

int winner(WINDOW *win, int l, int c, int **poz)
{
	//Avem un câștigător. Afișăm mesajul și așteptăm ca acesta să intre în meniu cu ':'.
	char s1[]="Felicitari, ai ghicit cuvantul!", s2[]="Doresti un joc nou? Apasa pe ':' pentru a alege.";
	int ch=0;
	wattron(win, COLOR_PAIR(2));
	mvwprintw(win, (poz[29][0]+l)/2 , (c-strlen(s1))/2, "%s", s1);
	mvwprintw(win, (poz[29][0]+l)/2+1, (c-strlen(s2))/2, "%s", s2);
	wattroff(win, COLOR_PAIR(2));
	wrefresh(win);
	while(ch!=':')
    	{
        	ch=wgetch(win);
    	}
	//Se va returna opțiunea aleasă în meniu.
    	return meniu_control(win,l,c,poz,0);
}

int loser(WINDOW *win, int l, int c, int **poz, char *word)
{
	//Avem un pierzător. Afișăm mesajul și așteptăm ca acesta să intre în meniu cu ':'.
 	char s[]="Ai pierdut! Cuvantul cautat era '", s1[]="Apasa ':' pentru a intra in meniu.";
 	int ch=0;
	init_pair(4, COLOR_RED, COLOR_WHITE);
	wattron(win, COLOR_PAIR(4));
	mvwprintw(win, (poz[29][0]+l)/2, (c-strlen(s)-strlen(word)+4)/2, "%s", strcat(strcat(s,word),"'."));
	mvwprintw(win, (poz[29][0]+l)/2+1, (c-strlen(s1))/2, "%s", s1);
	wattroff(win, COLOR_PAIR(4));
	wrefresh(win);
	while(ch!=':')
	{
		ch=wgetch(win);
	}
	//Se va returna opțiunea aleasă în meniu.
	return meniu_control(win,l,c,poz,0);
}

int check(WINDOW *win, char *c, char *b, int **poz, int try)
{
	/* Aici se verifică cât de aproape a fost utilizatorul de cuvântul câștigător.
	 * Dacă am aceeași literă pe aceeași poziție (c[i]==b[i]), dau color_pair(2), adică verde.
	 * Dacă litera există în cuvântul câștigător și nu este pe aceeași poziție (afbt[b[i]-'a']!=0), dau color_pair(3), adică galben.
	*/
	init_pair(2, COLOR_GREEN, COLOR_WHITE);
	init_pair(3, COLOR_YELLOW, COLOR_WHITE);
	int valid=1, i, afbt[27] = {0};
	wrefresh(win);
	for(i=0; i<5; i++)
	{
		afbt[c[i]-'a']++;
	}
	wattron(win, COLOR_PAIR(2));
	for(i=0; i<5; i++)
	{
		if(c[i]==b[i])
		{
			mvwaddch(win, poz[i+try*5][0], poz[i+try*5][1], b[i]-'a'+'A');
			afbt[c[i]-'a']--;
		}
		else
		{
			valid=0;
		}
	}
	wattroff(win, COLOR_PAIR(2));
	wattron(win, COLOR_PAIR(3));
	for(i=0; i<5; i++)
	{
		if(afbt[b[i]-'a']!=0)
		{
			mvwaddch(win, poz[i+try*5][0], poz[i+try*5][1], b[i]-'a'+'A');
			afbt[b[i]-'a']--;
		}
	}
	wattroff(win, COLOR_PAIR(3));
	//Returnez dacă cuvântul a fost găsit (1) sau nu (0).
	return valid;
}

int start_guessing(WINDOW *win, int l, int c, int **poz, char *word)
{
	/*Funcție pentru inserarea caracterelor în tabel și utilizarea backspace-ului, a enter-ului sau a meniului de control.
	* Funcția poate returna 0, adică lose, 1, adică win, 3, adică new game,  sau 4 , adică încheiereprematură a jocului. 
	*/
	int try=0, let=0, a=0, i;
	char b[6], s[]="Introdu 5 litere.";
	while(try!=6)
	{
		wmove(win,poz[let+try*5][0],poz[let+try*5][1]);
		a=wgetch(win);
		if((((a>='a')&&(a<='z'))||((a>='A')&&(a<='Z')))&&(let<5))
		{
			//Afișarea literei.
			if((a>='A')&&(a<='Z'))
			{
				a=a-'A'+'a';
			}
			b[let]=a;
			let++;
			mvwaddch(win,poz[let+try*5-1][0],poz[let+try*5-1][1], b[let-1]-'a'+'A');
			wrefresh(win);
		}
		if(a==KEY_BACKSPACE)
		{
			//Avem backspace. Scădem o poziție. Ștergem afișarea precedentă.
			if(let>0)
			{
				b[let-1]=' ';
				mvwaddch(win,poz[let+try*5-1][0],poz[let+try*5-1][1], b[let-1]);
				wrefresh(win);
				let--;
			}
		}
		if(a=='\n')
		{
			//Avem enter. Verificăm dacă avem 5 litere.
			if(let==5)
			{
				wmove(win,(poz[29][0]+l)/2,0);
				wclrtoeol(win);
				wrefresh(win);
				if(check(win,word,b,poz,try)==1)
				{
					return 1;
				}
				else
				{
					let=0;
					try++;
				}
			}
			else
			{
				mvwaddstr(win,(poz[29][0]+l)/2,(c-strlen(s))/2,s);
				wrefresh(win);
			}
		}
		if(a==':')
		{
			/*Avem cerere pentru meniul de control.
			 * Se verifică dacă nu cumva utilizatorul vrea să continue jocul sau vrea
			 * să închidă jocul/să înceapă un joc nou.
			 */
			a=meniu_control(win,l,c,poz,1)+2;
			if(a!=2)
			{
				return a;
			}
			else
			{
				for(i=0; i<(l/2-6)/2; i++)
				{
					wmove(win, i, 0);
					wclrtoeol(win);
				}
				wrefresh(win);
			}
		}
	}
	return 0;
}

int main()
{
	//Pregătesc fereastra.
	initscr();
	clear();
	int nrlin, nrcol;
	getmaxyx(stdscr,nrlin,nrcol);
	WINDOW *win=newwin(nrlin,nrcol,0,0);
	refresh();
	//Câteva verificări ce asigură o funcționare bună.
	if(!has_colors())
	{
		printw("Terminalul nu a reusit sa incarce culorile.");
		getch();
	}
	else if((nrcol<50)||(nrlin<45))
	{
		printw("Jocul nu s-a putut incarca din cauza dimensiunii ferestrei. Te rog sa maresti dimensiunea terminalului.");
		getch();
	}
	else
	{
		//Pregătesc și aloc dinamic vectorul de poziții de pe fereastra, poziții unde voi pune litere.
		FILE *f=fopen("word_list.in", "r");
		int i, **poz, n=5, choice=1;
		char c[6], **word_list;
		poz=(int **)malloc(31*sizeof(int *));
		for(i=0; i<31; i++)
		{
			poz[i]=(int *)malloc(2*sizeof(int));
		}
		word_list=(char **)malloc(21*sizeof(char *));
		i=0;
		//Extrag cuvintele din listă într-un vector de cuvinte.
		while(!feof(f))
		{
			if(i==n)
			{
				n++;
				word_list=(char **)realloc(word_list,n*sizeof(char *));
			}
			word_list[i]=(char *)malloc(6*sizeof(char));
			fscanf(f, "%s", word_list[i]);
			i++;
		}
		fclose(f);
		start_color();
		noecho();
		keypad(win,true);
		//choice îmi sugerează dacă utilizatorul continuă să joace sau nu.
		while(choice==1)
		{
			for(i=0; i<=nrlin; i++)
			{
				wmove(win,i,0);
				wclrtoeol(win);
			}
			initialize_game(win, nrlin, nrcol, poz);
			srand(time(0));
			strncpy(c,word_list[rand()%n],5);
			/*Pornim joaca. Primele 2 case-uri îmi indică un joc terminat, ultimele 2 o
			 * decizie prematură de încheiere a jocului nou, case 3 - new game, case 4 - stop joc.
			 */
			switch(start_guessing(win, nrlin, nrcol, poz, c))
			{
				case 0:
					choice=loser(win, nrlin, nrcol, poz, c);
					break;
				case 1:
					choice=winner(win, nrlin, nrcol, poz);
					break;
				case 3:
					choice=1;
					break;
				case 4:
					choice=2;
					break;
			}
		}
		for(i=0; i<n; i++)
		{
			free(word_list[i]);
		}
		for(i=0; i<31; i++)
		{
			free(poz[i]);
		}
		free(poz);
		free(word_list);
	}
	delwin(win);
	endwin();
	return 0;
}
