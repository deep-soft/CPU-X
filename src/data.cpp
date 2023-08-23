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
* FILE data.cpp
*/

#include <string>
#include <iomanip>
#include <memory>
#include "options.hpp"
#include "util.hpp"
#include "data.hpp"


/* struct Tab */

Tab::Tab(std::string name) : name(name)
{
}

std::ostream& operator<<(std::ostream& os, const Tab& tab)
{
	const char *col_start = Options::get_color() ? BOLD_BLUE : "";
	const char *col_end   = Options::get_color() ? DEFAULT   : "";
	os << "  " << col_start << ">>>>>>>>>> " << _(tab.name.c_str()) << " <<<<<<<<<<" << col_end << std::endl << std::endl;
	return os;
}


/* struct Frame */

Frame::Frame()
{
}

Frame::Frame(std::string name) : name(name)
{
}

std::ostream& operator<<(std::ostream& os, const Frame& frame)
{
	const char *col_start = Options::get_color() ? BOLD_BLUE : "";
	const char *col_end   = Options::get_color() ? DEFAULT   : "";
	os << "\t" << col_start << "***** " << _(frame.name.c_str()) << " *****" << col_end << std::endl;
	return os;
}


/* struct MetaFrame */

MetaFrame::MetaFrame(std::string name) : Frame(name)
{
}

std::ostream& operator<<(std::ostream& os, const MetaFrame& metaframe)
{
	const char *col_start = Options::get_color() ? BOLD_BLUE : "";
	const char *col_end   = Options::get_color() ? DEFAULT   : "";
	os << "    " << col_start << "####### " << _(metaframe.name.c_str()) << " #######" << col_end << std::endl;
	return os;
}


/* struct Label */

Label::Label(std::string name) : name(name), value("")
{
}


std::ostream& operator<<(std::ostream& os, const Label& label)
{
	os << std::setw(16) << _(label.name.c_str()) << ": " << label.value << std::endl;
	return os;
}


/* struct Data */

std::ostream& operator<<(std::ostream& os, const Data& data)
{
	os << static_cast<const Data::Cpu&>(data.cpu);
	os << static_cast<const Data::Caches&>(data.caches);
	os << static_cast<const Data::Motherboard&>(data.motherboard);
	os << static_cast<const Data::Memory&>(data.memory);
	os << static_cast<const Data::System&>(data.system);
	os << static_cast<const Data::Graphics&>(data.graphics);
	return os;
}

/*        Data::Cpu */

Data::Cpu::Cpu() : Tab(N_("CPU"))
{
}

void Data::Cpu::grow_cpu_types_vector(uint8_t index, const char *purpose)
{
	this->cpu_types.push_back({index, purpose});
}

uint16_t Data::Cpu::get_selected_core_id()
{
	return this->get_selected_cpu_type().footer.core_id_offset + Options::get_selected_core();
}

uint16_t Data::Cpu::get_first_core_id_for_type(uint8_t cpu_type)
{
	if(cpu_type < this->cpu_types.size())
		return this->cpu_types.at(cpu_type).footer.core_id_offset;
	else
		return 0;
}

Data::Cpu::CpuType& Data::Cpu::get_selected_cpu_type()
{
	/* Check that vector is not empty */
	if(this->cpu_types.size() == 0)
		this->grow_cpu_types_vector(0, _("unknown"));

	return this->cpu_types.at(Options::get_selected_type());
}

const std::string Data::Cpu::get_selected_cpu_type_formatted()
{
	return this->get_selected_cpu_type().name;
}

const std::string Data::Cpu::get_selected_cpu_core_formatted()
{
	return this->get_selected_cpu_type().get_core_type_name() + " #" + std::to_string(Options::get_selected_core());
}

std::ostream& operator<<(std::ostream& os, const Data::Cpu& cpu)
{
	os << static_cast<const Tab&>(cpu);
	os << static_cast<const Data::Cpu::Clocks&>(cpu.clocks);
	for(auto& cpu_type : cpu.cpu_types)
	{
		os << static_cast<const MetaFrame&>(cpu_type);
		os << static_cast<const Data::Cpu::CpuType&>(cpu_type);
	}
	os << std::endl;
	return os;
}

Data::Cpu::CpuType::CpuType(uint8_t index, const char *purpose) : MetaFrame(string_format(("#%u %s"), index, purpose))
{
}

const std::string Data::Cpu::CpuType::CpuType::get_core_type_name()
{
#if HAS_LIBCPUID
	switch(this->purpose)
	{
		case PURPOSE_PERFORMANCE: return _("P-core");
		case PURPOSE_EFFICIENCY:  return _("E-core");
		default:                  return _("Core");
	}
#endif /* HAS_LIBCPUID */
	return _("Core");
}

std::ostream& operator<<(std::ostream& os, const Data::Cpu::CpuType& cpu_type)
{
	os << static_cast<const Data::Cpu::CpuType::Processor&>(cpu_type.processor);
	os << static_cast<const Data::Cpu::CpuType::Caches&>(cpu_type.caches);
	os << static_cast<const Data::Cpu::CpuType::Footer&>(cpu_type.footer);
	return os;
}

Data::Cpu::CpuType::Processor::Processor() : Frame(N_("Processor"))
{
}

std::string Data::Cpu::CpuType::Processor::format_cpuid_value(int32_t value)
{
	if(Options::get_cpuid_decimal())
		return string_format("%i", value);
	else
		return string_format("0x%X", value);
}

std::ostream& operator<<(std::ostream& os, const Data::Cpu::CpuType::Processor& processor)
{
	os << static_cast<const Frame&>(processor);
	os << static_cast<const Label&>(processor.vendor);
	os << static_cast<const Label&>(processor.codename);
	os << static_cast<const Label&>(processor.package);
	os << static_cast<const Label&>(processor.technology);
	os << static_cast<const Label&>(processor.voltage);
	os << static_cast<const Label&>(processor.specification);
	os << static_cast<const Label&>(processor.family);
	os << static_cast<const Label&>(processor.dispfamily);
	os << static_cast<const Label&>(processor.model);
	os << static_cast<const Label&>(processor.dispmodel);
	os << static_cast<const Label&>(processor.temperature);
	os << static_cast<const Label&>(processor.stepping);
	os << static_cast<const Label&>(processor.instructions);
	os << std::endl;
	return os;
}

Data::Cpu::CpuType::Caches::Caches() : Frame(N_("Caches"))
{
}

std::string Data::Cpu::CpuType::Caches::format_cache_level(int32_t instance_count, int32_t cache_size, char *unit, int32_t cache_assoc)
{
	if(instance_count > 1)
		return string_format(_("%2d x %4d %s, %2d-way"), instance_count, cache_size, unit, cache_assoc);
	else
		return string_format(_("%d %s, %d-way"), cache_size, unit, cache_assoc);
}

std::ostream& operator<<(std::ostream& os, const Data::Cpu::CpuType::Caches& caches)
{
	os << static_cast<const Frame&>(caches);
	os << static_cast<const Label&>(caches.level1d);
	os << static_cast<const Label&>(caches.level1i);
	os << static_cast<const Label&>(caches.level2);
	os << static_cast<const Label&>(caches.level3);
	os << std::endl;
	return os;
}

Data::Cpu::CpuType::Footer::Footer() : Frame(N_("Count"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::Cpu::CpuType::Footer& footer)
{
	os << static_cast<const Frame&>(footer);
	os << static_cast<const Label&>(footer.cores);
	os << static_cast<const Label&>(footer.threads);
	os << std::endl;
	return os;
}

Data::Cpu::Clocks::Clocks() : Frame(N_("Clocks"))
{
}

void Data::Cpu::Clocks::set_cpu_freq(int cpu_freq)
{
	this->cpu_freq         = cpu_freq;
	this->core_speed.value = string_format("%4i MHz", this->cpu_freq);
}

void Data::Cpu::Clocks::set_bus_freq(double bus_freq)
{
	this->bus_freq        = bus_freq;
	this->bus_speed.value = string_format("%.2f MHz", this->bus_freq);
}

int Data::Cpu::Clocks::set_cpu_multiplier()
{
	if((this->cpu_freq <= 0) || (this->bus_freq <= 0.0))
		return 1;

	const int    fmt_min  = (this->cpu_min_mult < 10) ? 1 : 0;
	const int    fmt_max  = (this->cpu_max_mult < 10) ? 1 : 0;
	const double cur_mult = double(this->cpu_freq) / this->bus_freq;

	if((this->cpu_min_mult <= 0.0) && (this->cpu_max_mult <= 0.0))
		this->multiplier.value = string_format("x %.2f", cur_mult);
	else
	{
		this->multiplier.value  = string_format("x%4.1f", cur_mult);
		this->multiplier.value += " (";
		this->multiplier.value += (this->cpu_min_mult <= 0.0) ? "?" : string_format("%.*f", fmt_min, this->cpu_min_mult);
		this->multiplier.value += "-";
		this->multiplier.value += (this->cpu_max_mult <= 0.0) ? "?" : string_format("%.*f", fmt_max, this->cpu_max_mult);
		this->multiplier.value += ")";
	}

	return 0;
}

std::ostream& operator<<(std::ostream& os, const Data::Cpu::Clocks& clocks)
{
	os << static_cast<const Frame&>(clocks);
	os << static_cast<const Label&>(clocks.core_speed);
	os << static_cast<const Label&>(clocks.multiplier);
	os << static_cast<const Label&>(clocks.bus_speed);
	os << static_cast<const Label&>(clocks.usage);
	os << std::endl;
	return os;
}

/*        Data::Caches */

Data::Caches::Caches() : Tab(N_("Caches"))
{
}

void Data::Caches::grow_cpu_types_vector(uint8_t index, const char *purpose)
{
	this->cpu_types.push_back({index, purpose});
}

Data::Caches::CpuType& Data::Caches::get_selected_cpu_type()
{
	/* Check that vector is not empty */
	if(this->cpu_types.size() == 0)
		this->grow_cpu_types_vector(0, _("unknown"));

	return this->cpu_types.at(Options::get_selected_type());
}

std::ostream& operator<<(std::ostream& os, const Data::Caches& caches)
{
	if(caches.cpu_types.size() > 0)
	{
		os << static_cast<const Tab&>(caches);
		for(auto& cpu_type : caches.cpu_types)
		{
			os << static_cast<const MetaFrame&>(cpu_type);
			os << static_cast<const Data::Caches::CpuType&>(cpu_type);
		}
		os << std::endl;
	}
	return os;
}

Data::Caches::CpuType::CpuType(uint8_t index, const char *purpose) : MetaFrame(string_format(("#%u %s"), index, purpose))
{
}

void Data::Caches::CpuType::grow_caches_vector_with_cache_size(uint8_t level, uint32_t size_i, const char *cache_size, int32_t cache_line, char *unit)
{
	this->caches.push_back({level, size_i, cache_size, cache_line, unit});
}

std::ostream& operator<<(std::ostream& os, const Data::Caches::CpuType& cpu_type)
{
	if(cpu_type.caches.size() > 0)
		for(auto& cache : cpu_type.caches)
			os << static_cast<const Data::Caches::CpuType::Cache&>(cache);
	return os;
}

Data::Caches::CpuType::Cache::Cache(uint8_t level) : Frame(string_format(N_("L%u Cache"), level))
{
}

Data::Caches::CpuType::Cache::Cache(uint8_t level, uint32_t size_i, const char *cache_size, int32_t cache_line, char *unit) : Frame(string_format(N_("L%u Cache"), level))
{
	this->size.value = string_format(_("%s associative, %d-%s line size"), cache_size, cache_line, unit);
	this->size_i     = size_i;
}

std::ostream& operator<<(std::ostream& os, const Data::Caches::CpuType::Cache& cache)
{
	os << static_cast<const Frame&>(cache);
	os << static_cast<const Label&>(cache.size);
	os << static_cast<const Label&>(cache.speed);
	os << std::endl;
	return os;
}

Data::Caches::Test::Test() : Frame(N_("Test"))
{
}

const std::string Data::Caches::Test::get_selected_test_formatted()
{
	return this->names.at(Options::get_selected_test());
}

/*        Data::Motherboard */

Data::Motherboard::Motherboard() : Tab(N_("Motherboard"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::Motherboard& motherboard)
{
	os << static_cast<const Tab&>(motherboard);
	os << static_cast<const Data::Motherboard::Board&>(motherboard.board);
	os << static_cast<const Data::Motherboard::Bios&>(motherboard.bios);
	os << static_cast<const Data::Motherboard::Chipset&>(motherboard.chipset);
	os << std::endl;
	return os;
}

Data::Motherboard::Board::Board() : Frame(N_("Motherboard"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::Motherboard::Board& board)
{
	os << static_cast<const Frame&>(board);
	os << static_cast<const Label&>(board.manufacturer);
	os << static_cast<const Label&>(board.model);
	os << static_cast<const Label&>(board.revision);
	os << std::endl;
	return os;
}

Data::Motherboard::Bios::Bios() : Frame(N_("BIOS"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::Motherboard::Bios& bios)
{
	os << static_cast<const Frame&>(bios);
	os << static_cast<const Label&>(bios.brand);
	os << static_cast<const Label&>(bios.version);
	os << static_cast<const Label&>(bios.date);
	os << static_cast<const Label&>(bios.romsize);
	os << std::endl;
	return os;
}

Data::Motherboard::Chipset::Chipset() : Frame(N_("Chipset"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::Motherboard::Chipset& chipset)
{
	os << static_cast<const Frame&>(chipset);
	os << static_cast<const Label&>(chipset.vendor);
	os << static_cast<const Label&>(chipset.model);
	os << std::endl;
	return os;
}

/*        Data::Memory */

Data::Memory::Memory() : Tab(N_("Memory"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::Memory& memory)
{
	if(memory.dimms.size() > 0)
	{
		os << static_cast<const Tab&>(memory);
		for(auto& dimm : memory.dimms)
			os << static_cast<const Data::Memory::Dimm&>(dimm);
		os << std::endl;
	}
	return os;
}

Data::Memory::Dimm::Dimm(uint8_t number) : Frame(string_format(N_("Bank %u"), number))
{
}

void Data::Memory::grow_dimms_vector()
{
	const auto len = this->dimms.size();
	this->dimms.push_back(len);
}

std::ostream& operator<<(std::ostream& os, const Data::Memory::Dimm& dimm)
{
	os << static_cast<const Frame&>(dimm);
	os << static_cast<const Label&>(dimm.reference);
	os << std::endl;
	return os;
}

/*        Data::System */

Data::System::System() : Tab(N_("System"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::System& system)
{
	os << static_cast<const Tab&>(system);
	os << static_cast<const Data::System::OperatingSystem&>(system.os);
	os << static_cast<const Data::System::Memory&>(system.memory);
	os << std::endl;
	return os;
}

Data::System::OperatingSystem::OperatingSystem() : Frame(N_("Operating System"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::System::OperatingSystem& operatingsystem)
{
	os << static_cast<const Frame&>(operatingsystem);
	os << static_cast<const Label&>(operatingsystem.kernel);
	os << static_cast<const Label&>(operatingsystem.distribution);
	os << static_cast<const Label&>(operatingsystem.hostname);
	os << static_cast<const Label&>(operatingsystem.uptime);
	os << static_cast<const Label&>(operatingsystem.compiler);
	os << std::endl;
	return os;
}

Data::System::Memory::Memory() : Frame(N_("Memory"))
{
}

std::ostream& operator<<(std::ostream& os, const Data::System::Memory& memory)
{
	os << static_cast<const Frame&>(memory);
	os << static_cast<const Label&>(memory.used);
	os << static_cast<const Label&>(memory.buffers);
	os << static_cast<const Label&>(memory.cached);
	os << static_cast<const Label&>(memory.free);
	os << static_cast<const Label&>(memory.swap);
	os << std::endl;
	return os;
}

/*        Data::Graphics */

Data::Graphics::Graphics() : Tab(N_("Graphics"))
{
}

void Data::Graphics::grow_cards_vector()
{
	const auto len = this->cards.size();
	this->cards.push_back(len);
}

Data::Graphics::Card& Data::Graphics::get_selected_card()
{
	/* Check selected type is a valid index */
	Options::set_selected_gpu(Options::get_selected_gpu(), this->cards.size());

	/* Check that vector is not empty */
	if(this->cards.size() == 0)
		this->grow_cards_vector();

	return this->cards.at(Options::get_selected_gpu());
}

std::ostream& operator<<(std::ostream& os, const Data::Graphics& graphics)
{
	if(graphics.cards.size() > 0)
	{
		os << static_cast<const Tab&>(graphics);
		for(auto& card : graphics.cards)
			os << static_cast<const Data::Graphics::Card&>(card);
		os << std::endl;
	}
	return os;
}

Data::Graphics::Card::Card(uint8_t number) : Frame(string_format(N_("Card %u"), number))
{
}

std::ostream& operator<<(std::ostream& os, const Data::Graphics::Card& card)
{
	os << static_cast<const Frame&>(card);
	os << static_cast<const Label&>(card.vendor);
	os << static_cast<const Label&>(card.kernel_driver);
	os << static_cast<const Label&>(card.user_mode_driver);
	os << static_cast<const Label&>(card.model);
	os << static_cast<const Label&>(card.comp_unit);
	os << static_cast<const Label&>(card.device_id);
	os << static_cast<const Label&>(card.vbios_version);
	os << static_cast<const Label&>(card.interface);
	os << static_cast<const Label&>(card.temperature);
	os << static_cast<const Label&>(card.usage);
	os << static_cast<const Label&>(card.core_voltage);
	os << static_cast<const Label&>(card.power_avg);
	os << static_cast<const Label&>(card.core_clock);
	os << static_cast<const Label&>(card.mem_clock);
	os << static_cast<const Label&>(card.mem_used);
	os << static_cast<const Label&>(card.resizable_bar);
	os << static_cast<const Label&>(card.vulkan_rt);
	os << static_cast<const Label&>(card.opengl_version);
	os << static_cast<const Label&>(card.vulkan_version);
	os << static_cast<const Label&>(card.opencl_version);
	os << std::endl;
	return os;
}

const std::string Data::Graphics::get_selected_card_formatted()
{
	return "#" + std::to_string(Options::get_selected_gpu()) + ": " + this->get_selected_card().model.value;
}

/*        Data::Bench */

Data::Bench::Bench() : Tab(N_("Bench"))
{
}

Data::Bench::~Bench()
{
	/* Cleanup done by stop_benchmarks() */
	if(this->is_running)
		this->is_running = false;
}

Data::Bench::PrimeNumbers::PrimeNumbers(std::string name) : Frame(name)
{
	this->score.value = _("Not started");
}

Data::Bench::PrimeSlow::PrimeSlow() : PrimeNumbers(N_("Prime numbers (slow)"))
{
}

Data::Bench::PrimeFast::PrimeFast() : PrimeNumbers(N_("Prime numbers (fast)"))
{
}

Data::Bench::Parameters::Parameters() : Frame(N_("Parameters"))
{
}

bool Data::Bench::Parameters::set_threads(uint_fast16_t threads)
{
	if((threads > 0) && (threads <= std::thread::hardware_concurrency()))
	{
		this->threads_i     = threads;
		this->threads.value = std::to_string(this->threads_i);
		return true;
	}
	else
	{
		this->threads_i     = 1;
		this->threads.value = std::to_string(this->threads_i);
		return false;
	}
}

bool Data::Bench::Parameters::set_duration(uint_fast32_t duration)
{
	if((duration > 0) && (duration < 60 * 24))
	{
		this->duration_i     = duration;
		this->duration.value = string_format(_("%u mins"), this->duration_i);
		return true;
	}
	else
	{
		this->duration_i     = 1;
		this->duration.value = string_format(_("%u mins"), this->duration_i);
		return false;
	}
}

/*        Data::About */

Data::About::About() : Tab(N_("About"))
{
}

Data::About::Description::Description() :
	text(string_format(N_("%s is a Free software that gathers information\n"
	     "on CPU, motherboard and more."), PRGNAME))
{
}

Data::About::AboutCpuX::AboutCpuX() : Frame(N_("About CPU-X")),
	version(string_format(N_("Version %s"), PRGVER)),
	author(string_format(N_("Author: %s"), PRGAUTH)),
	website(string_format(N_("Site: %s"), PRGURL))
{
}

Data::About::License::License() : Frame(N_("License")),
	copyright(PRGCPRGHT),
	name(string_format(N_("This software is distributed under the terms of %s"), PRGLCNS)),
	warranty(N_("This program comes with ABSOLUTELY NO WARRANTY"))
{
}
