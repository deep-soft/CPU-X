/****************************************************************************
*    Copyright © 2014-2023 The Tumultuous Unicorn Of Darkness
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/*
* PROJECT CPU-X
* FILE data.hpp
*/

#ifndef _DATA_HPP_
#define _DATA_HPP_

#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <thread>
#include <atomic>
#include "util.hpp"

#if HAS_LIBCPUID
# include <libcpuid/libcpuid.h>
#endif


struct Tab
{
	const std::string name;

	Tab(std::string name);
	friend std::ostream& operator<<(std::ostream& os, const Tab& t);
};

struct Frame
{
	const std::string name;

	Frame();
	Frame(std::string name);
	friend std::ostream& operator<<(std::ostream& os, const Frame& frame);
};

struct MetaFrame : public Frame
{
	MetaFrame(std::string name);
	friend std::ostream& operator<<(std::ostream& os, const MetaFrame& metaframe);
};

struct Label
{
	const std::string name;
	std::string value;

	Label(std::string name);
	friend std::ostream& operator<<(std::ostream& os, const Label& label);
};

struct Data
{
	int socket_fd = -1;

	struct Cpu : public Tab
	{
		char *cpuid_raw_file = NULL;
#if HAS_LIBCPUID
		cpu_vendor_t vendor;
		int32_t ext_family;
#endif /* HAS_LIBCPUID */

		struct CpuType : public MetaFrame
		{
#if HAS_LIBCPUID
			cpu_purpose_t purpose;
#endif /* HAS_LIBCPUID */

			struct Processor : public Frame
			{
				Label vendor        {N_("Vendor")};
				Label codename      {N_("Code Name")};
				Label package       {N_("Package")};
				Label technology    {N_("Technology")};
				Label voltage       {N_("Voltage")};
				Label specification {N_("Specification")};
				Label family        {N_("Family")};
				Label dispfamily    {N_("Disp. Family")};
				Label model         {N_("Model")};
				Label dispmodel     {N_("Disp. Model")};
				Label temperature   {N_("Temp.")};
				Label stepping      {N_("Stepping")};
				Label instructions  {N_("Instructions")};
				std::string path_cpu_temperature {};
				std::string path_cpu_voltage     {};

				Processor();
				static std::string format_cpuid_value(int32_t value);
				friend std::ostream& operator<<(std::ostream& os, const Processor& processor);
			} processor;

			struct Caches : public Frame
			{
				Label level1d {N_("L1 Data")};
				Label level1i {N_("L1 Inst.")};
				Label level2  {N_("Level 2")};
				Label level3  {N_("Level 3")};

				Caches();
				static std::string format_cache_level(int32_t instance_count, int32_t cache_size, char *unit, int32_t cache_assoc);
				friend std::ostream& operator<<(std::ostream& os, const Caches& caches);
			} caches;

			struct Footer : public Frame
			{
				Label cores   {N_("Cores")};
				Label threads {N_("Threads")};
				uint16_t num_threads    = 0;
				uint16_t core_id_offset = 0;

				Footer();
				friend std::ostream& operator<<(std::ostream& os, const Footer& footer);
			} footer;

			CpuType(uint8_t index, const char *purpose);
			const std::string get_core_type_name();
			friend std::ostream& operator<<(std::ostream& os, const CpuType& cpu_type);
		};
		std::vector<CpuType> cpu_types {};

		struct Clocks : public Frame
		{
			Label core_speed  {N_("Core Speed")};
			Label multiplier  {N_("Multiplier")};
			Label bus_speed   {N_("Bus Speed")};
			Label usage       {N_("Usage")};
			int cpu_freq        = 0;
			double bus_freq     = 0.0;
			double cpu_min_mult = 0.0;
			double cpu_max_mult = 0.0;
			std::vector<long> cpu_time_stat;

			Clocks();
			void set_cpu_freq(int cpu_freq);
			void set_bus_freq(double bus_freq);
			int set_cpu_multiplier();
			friend std::ostream& operator<<(std::ostream& os, const Clocks& clocks);
		} clocks;

		Cpu();
		void grow_cpu_types_vector(uint8_t index, const char *purpose);
		uint16_t get_selected_core_id();
		uint16_t get_first_core_id_for_type(uint8_t cpu_type);
		CpuType& get_selected_cpu_type();
		const std::string get_selected_cpu_type_formatted();
		const std::string get_selected_cpu_core_formatted();
		friend std::ostream& operator<<(std::ostream& os, const Cpu& cpu);
	} cpu;

	struct Caches : public Tab
	{
		struct CpuType : public MetaFrame
		{
			struct Cache : public Frame
			{
				Label size  {N_("Size")};
				Label speed {N_("Speed")};
				uint32_t size_i, speed_i;

				Cache(uint8_t level);
				Cache(uint8_t level, uint32_t size_i, const char *cache_size, int32_t cache_line, char *unit);
				friend std::ostream& operator<<(std::ostream& os, const Cache& cache);
			};
			std::vector<Cache> caches {};

			CpuType(uint8_t index, const char *purpose);
			void grow_caches_vector_with_cache_size(uint8_t level, uint32_t size_i, const char *cache_size, int32_t cache_line, char *unit);
			friend std::ostream& operator<<(std::ostream& os, const CpuType& cpu_type);
		};
		std::vector<CpuType> cpu_types {};

		struct Test : public Frame
		{
			std::vector<std::string> names {};

			Test();
			const std::string get_selected_test_formatted();
		} test;

		Caches();
		void grow_cpu_types_vector(uint8_t index, const char *purpose);
		CpuType& get_selected_cpu_type();
		friend std::ostream& operator<<(std::ostream& os, const Caches& caches);
	} caches;

	struct Motherboard : public Tab
	{
		struct Board : public Frame
		{
			Label manufacturer {N_("Manufacturer")};
			Label model        {N_("Model")};
			Label revision     {N_("Revision")};

			Board();
			friend std::ostream& operator<<(std::ostream& os, const Board& board);
		} board;

		struct Bios : public Frame
		{
			Label brand   {N_("Brand")};
			Label version {N_("Version")};
			Label date    {N_("Date")};
			Label romsize {N_("ROM Size")};

			Bios();
			friend std::ostream& operator<<(std::ostream& os, const Bios& bios);
		} bios;

		struct Chipset : public Frame
		{
			Label vendor {N_("Vendor")};
			Label model  {N_("Model")};

			Chipset();
			friend std::ostream& operator<<(std::ostream& os, const Chipset& chipset);
		} chipset;

		Motherboard();
		friend std::ostream& operator<<(std::ostream& os, const Motherboard& motherboard);
	} motherboard;

	struct Memory : public Tab
	{
		struct Dimm : public Frame
		{
			Label reference {N_("Reference")};

			Dimm(uint8_t number);
			friend std::ostream& operator<<(std::ostream& os, const Dimm& dimm);
		};
		std::vector<Dimm> dimms {};

		Memory();
		void grow_dimms_vector();
		friend std::ostream& operator<<(std::ostream& os, const Memory& memory);
	} memory;

	struct System : public Tab
	{
		struct OperatingSystem : public Frame
		{
			Label kernel       {N_("Kernel")};
			Label distribution {N_("Distribution")};
			Label hostname     {N_("Hostname")};
			Label uptime       {N_("Uptime")};
			Label compiler     {N_("Compiler")};

			OperatingSystem();
			friend std::ostream& operator<<(std::ostream& os, const OperatingSystem& operatingsystem);
		} os;

		struct Memory : public Frame
		{
			Label used    {N_("Used")};
			Label buffers {N_("Buffers")};
			Label cached  {N_("Cached")};
			Label free    {N_("Free")};
			Label swap    {N_("Swap")};
			long double mem_used, mem_buffers, mem_cached, mem_free, mem_total;
			long double swap_used, swap_total;

			Memory();
			friend std::ostream& operator<<(std::ostream& os, const Memory& memory);
		} memory;

		System();
		friend std::ostream& operator<<(std::ostream& os, const System& system);
	} system;

	struct Graphics : public Tab
	{
		struct Card : public Frame
		{
			enum GpuDrv
			{
				GPUDRV_FGLRX, GPUDRV_AMDGPU, GPUDRV_RADEON,                                       // AMD
				GPUDRV_INTEL,                                                                     // Intel
				GPUDRV_NVIDIA, GPUDRV_NVIDIA_BUMBLEBEE, GPUDRV_NOUVEAU, GPUDRV_NOUVEAU_BUMBLEBEE, // NVIDIA
				GPUDRV_VFIO,
				GPUDRV_UNKNOWN
			};

			Label vendor           {N_("Vendor")};
			Label kernel_driver    {N_("Driver")};
			Label user_mode_driver {N_("UMD Version")};
			Label model            {N_("Model")};
			Label comp_unit        {N_("Compute Unit")};
			Label device_id        {N_("Device ID")};
			Label vbios_version    {N_("VBIOS Version")};
			Label interface        {N_("Interface")};
			Label temperature      {N_("Temperature")};
			Label usage            {N_("Usage")};
			Label core_voltage     {N_("Core Voltage")};
			Label power_avg        {N_("Power Avg")};
			Label core_clock       {N_("Core Clock")};
			Label mem_clock        {N_("Memory Clock")};
			Label mem_used         {N_("Memory Used")};
			Label resizable_bar    {N_("Resizable BAR")};
			Label vulkan_rt        {N_("Vulkan RT")};
			Label opengl_version   {N_("OpenGL Version")};
			Label vulkan_version   {N_("Vulkan Version")};
			Label opencl_version   {N_("OpenCL Version")};
			int drm_card_number    = -1;
			uint64_t vram_size     = 0;
			std::string device_path{};
			std::string drm_path   {};
			std::string hwmon_path {};
			GpuDrv driver          {GPUDRV_UNKNOWN};

			Card(uint8_t number);
			friend std::ostream& operator<<(std::ostream& os, const Card& card);
		};
		std::vector<Card> cards {};

		struct Footer : public Frame
		{
		} footer;

		Graphics();
		void grow_cards_vector();
		Card& get_selected_card();
		const std::string get_selected_card_formatted();
		friend std::ostream& operator<<(std::ostream& os, const Graphics& graphics);
	} graphics;

	struct Bench : public Tab
	{
		bool did_run    = false;
		bool is_running = false;
		bool fast_mode  = false;
		std::vector<std::thread> compute_threads {};

		struct PrimeNumbers : public Frame
		{
			Label score {N_("Score")};
			Label state {N_("Run")};
			std::atomic_uint_fast32_t primes = 0;
			std::atomic_uint_fast64_t number = 0;

			PrimeNumbers(std::string name);
		};

		struct PrimeSlow : public PrimeNumbers
		{
			PrimeSlow();
		} prime_slow;

		struct PrimeFast : public PrimeNumbers
		{
			PrimeFast();
		} prime_fast;

		struct Parameters : public Frame
		{
			Label duration{N_("Duration")};
			Label threads {N_("Threads")};
			uint_fast16_t threads_i  = 1;
			uint_fast32_t elapsed_i  = 0;
			uint_fast32_t duration_i = 1;

			Parameters();
			bool set_threads(uint_fast16_t threads);
			bool set_duration(uint_fast32_t duration);
		} parameters;

		Bench();
		~Bench();
	} bench;

	struct About : public Tab
	{
		struct Description : public Frame
		{
			const std::string text;

			Description();
		} description;

		struct AboutCpuX : public Frame
		{
			const std::string version;
			const std::string author;
			const std::string website;

			AboutCpuX();
		} about;

		struct License : public Frame
		{
			const std::string copyright;
			const std::string name;
			const std::string warranty;

			License();
		} license;

		About();
	} about;

	friend std::ostream& operator<<(std::ostream& os, const Data& data);
};


#endif /* _DATA_HPP_ */
