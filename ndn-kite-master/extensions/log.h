#include <fstream>
#include <unistd.h>
class Log {
public:
	static void  write_ts_to_log_file( const std::string &text )
	{
	    std::ofstream log_file(
	        "/home/vartika-kite/ndn-kite-master/results/res/kite_ts_log_file.txt",
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
		int count = read_cnt("/home/vartika-kite/ndn-kite-master/results/res/global_tracing_interest_counter.txt");
		std::ostringstream oss1;
		count+=1;
		//oss1<< "Tracing Interest (Interest for data): " << count;
		oss1<< count;
		std::string tsLog1(oss1.str());
		std::ofstream log_file("/home/vartika-kite/ndn-kite-master/results/res/global_tracing_interest_counter.txt", std::ios_base::out | std::ios_base::trunc);
		log_file << tsLog1 << std::endl;
	}

	static int write_traced_int_count()
	{
		int count = read_cnt("/home/vartika-kite/ndn-kite-master/results/res/global_traced_interest_counter.txt");
		std::ostringstream oss1;
		count+=1;
		oss1<< count;
		std::string tsLog1(oss1.str());
		std::ofstream log_file("/home/vartika-kite/ndn-kite-master/results/res/global_traced_interest_counter.txt", std::ios_base::out | std::ios_base::trunc);
		log_file << tsLog1 << std::endl;
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

