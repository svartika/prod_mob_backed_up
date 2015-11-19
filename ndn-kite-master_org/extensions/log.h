#include <fstream>

class Log {
public:
	static void  write_ts_to_log_file( const std::string &text )
	{
	    std::ofstream log_file(
	        "/home/vartika-kite/kite_ts_log_file.txt",
	        std::ios_base::out | std::ios_base::app );
	    log_file << text << std::endl;
	}

	static void  write_to_tracing_interest_tracker_node_n( const std::string &text , const char* fileName)
	{
		std::ofstream log_file(
				fileName,
				std::ios_base::out | std::ios_base::trunc );
		log_file << text << std::endl;
	}
	static void  write_to_traced_interest_tracker_node_n( const std::string &text , const char* fileName)
	{
		std::ofstream log_file(
				fileName,
				std::ios_base::out | std::ios_base::trunc );
		log_file << text << std::endl;
	}
	//20151001
	static void write_to_on_data_tracker(const std::string& text, const char* fileName)
	{
		std::ofstream log_file(fileName, std::ios_base::out | std::ios_base::trunc);
		log_file << text << std::endl;
	}

	static int read_cnt(const char* fileName)
	{
		std::ifstream readFile;
		readFile.open(fileName);
		char output[100];
		int i=0, loop=0;
		if (readFile.is_open())
		{
			while (!readFile.eof())
			{
				readFile >> output;
				std::cout<<"read from file: " << output << " - loop: " << loop++ <<"\n";
				i = atoi(output);//[0]-'0';
			}
		}
		readFile.close();
		return i;
	}

	static int write_tracing_int_count()
	{
		int count = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/global_tracing_interest_counter.txt");
		std::ostringstream oss1;
		count+=1;
		//oss1<< "Tracing Interest (Interest for data): " << count;
		oss1<< count;
		std::string tsLog1(oss1.str());
		std::ofstream log_file("/home/vartika-kite/ndn-kite-master_org/results/res/global_tracing_interest_counter.txt", std::ios_base::out | std::ios_base::trunc);
		log_file << tsLog1 << std::endl;
	}

	static int write_traced_int_count()
	{
		int count = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/global_traced_interest_counter.txt");
		std::ostringstream oss1;
		count+=1;
		oss1<< count;
		std::string tsLog1(oss1.str());
		std::ofstream log_file("/home/vartika-kite/ndn-kite-master_org/results/res/global_traced_interest_counter.txt", std::ios_base::out | std::ios_base::trunc);
		log_file << tsLog1 << std::endl;
	}

	static int write_int_count()
	{
		int count = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/global_regular_interest_counter.txt");
		std::ostringstream oss1;
		count+=1;
		oss1<< count;
		std::string tsLog1(oss1.str());
		std::ofstream log_file("/home/vartika-kite/ndn-kite-master_org/results/res/global_regular_interest_counter.txt", std::ios_base::out | std::ios_base::trunc);
		log_file << tsLog1 << std::endl;
	}

	static int write_retransmitted_interests()
	{
		int count = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/cons_retransmitted_interests.txt");
		std::ostringstream oss1;
		count+=1;
		oss1<< count;
		std::string tsLog1(oss1.str());
		std::ofstream log_file("/home/vartika-kite/ndn-kite-master_org/results/res/cons_retransmitted_interests.txt", std::ios_base::out | std::ios_base::trunc);
		log_file << tsLog1 << std::endl;
	}

	static int write_org_interests()
	{
		int count = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/cons_org_interests.txt");
		std::ostringstream oss1;
		count+=1;
		oss1<< count;
		std::string tsLog1(oss1.str());
		std::ofstream log_file("/home/vartika-kite/ndn-kite-master_org/results/res/cons_org_interests.txt", std::ios_base::out | std::ios_base::trunc);
		log_file << tsLog1 << std::endl;

	}

	static int store_to_another_file()
	{
		int g_traced, g_tracing, g_reg, sent, processed, received, retransmitted, org;

		g_traced = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/global_traced_interest_counter.txt");
		g_tracing = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/global_tracing_interest_counter.txt");
		g_reg = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/global_regular_interest_counter.txt");
		sent = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/consumer_sent_tracing.txt");
		processed = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/producer_process_tracing.txt");
		received = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/consumer_receives_data.txt");
		retransmitted = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/cons_retransmitted_interests.txt");
		org = read_cnt("/home/vartika-kite/ndn-kite-master_org/results/res/cons_org_interests.txt");

		std::ostringstream oss1;
		oss1<< "\n\n\ng_traced: "<< g_traced << "\ng_tracing: "<< g_tracing <<  "\nsent: "<< sent
				<< "\nprocessed: "<< processed << "\nreceived: "<< received<<"\ng_reg: "<< g_reg << "\nretransmitted: " << retransmitted << "\norg: "<<org;
		std::string tsLog1(oss1.str());

		std::ofstream log_file("/home/vartika-kite/ndn-kite-master_org/results/res/final_results.txt", std::ios_base::out | std::ios_base::app);
		log_file << tsLog1 << std::endl;
	}

	static int write_0_to_logs()
	{
		std::ostringstream oss1;
		int i=0;
		oss1<< i;
		std::string tsLog1(oss1.str());

		store_to_another_file();

		std::ofstream log_file1("/home/vartika-kite/ndn-kite-master_org/results/res/global_tracing_interest_counter.txt", std::ios_base::out | std::ios_base::trunc);
		log_file1 << tsLog1 << std::endl;

		std::ofstream log_file2("/home/vartika-kite/ndn-kite-master_org/results/res/global_traced_interest_counter.txt", std::ios_base::out | std::ios_base::trunc);
		log_file2 << tsLog1 << std::endl;

		std::ofstream log_file6("/home/vartika-kite/ndn-kite-master_org/results/res/global_regular_interest_counter.txt", std::ios_base::out | std::ios_base::trunc);
		log_file6 << tsLog1 << std::endl;

		std::ofstream log_file3("/home/vartika-kite/ndn-kite-master_org/results/res/consumer_sent_tracing.txt", std::ios_base::out | std::ios_base::trunc);
		log_file3 << tsLog1 << std::endl;

		std::ofstream log_file4("/home/vartika-kite/ndn-kite-master_org/results/res/producer_process_tracing.txt", std::ios_base::out | std::ios_base::trunc);
		log_file4 << tsLog1 << std::endl;

		std::ofstream log_file5("/home/vartika-kite/ndn-kite-master_org/results/res/consumer_receives_data.txt", std::ios_base::out | std::ios_base::trunc);
		log_file5 << tsLog1 << std::endl;

		std::ofstream log_file7("/home/vartika-kite/ndn-kite-master_org/results/res/cons_retransmitted_interests.txt", std::ios_base::out | std::ios_base::trunc);
		log_file7 << tsLog1 << std::endl;

		std::ofstream log_file8("/home/vartika-kite/ndn-kite-master_org/results/res/cons_org_interests.txt", std::ios_base::out | std::ios_base::trunc);
		log_file8 << tsLog1 << std::endl;

	}

	/*static void  write_to_tracing_interest_tracker( const std::string &text )
	{
		std::ofstream log_file(
				"/home/vartika-kite/kite_log_tracing.txt",
				std::ios_base::out | std::ios_base::trunc );
		log_file << text << std::endl;
	}
	static void  write_to_traced_interest_tracker( const std::string &text )
	{
		std::ofstream log_file(
				"/home/vartika-kite/kite_log_traced.txt",
				std::ios_base::out | std::ios_base::trunc );
		log_file << text << std::endl;
	}*/

};

