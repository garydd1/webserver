/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_readCgi.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcereced <vcereced@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 19:19:13 by vcereced          #+#    #+#             */
/*   Updated: 2024/06/20 19:31:42 by vcereced         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

void dupAndCloseFd(int *pipefd)
{
    close(pipefd[0]);
    
    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
        std::cerr << "dup2 failed: " << std::endl;
        _exit(EXIT_FAILURE);
    }
    close(pipefd[1]);
}

std::string readOutputCgi(std::string filePath)
{
    std::array<char, 2048>  buffer;
    std::string             result;
    ssize_t                 bytesRead;
    pid_t                   pid;
    int                     pipefd[2];
    int                     status;
    
    if (pipe(pipefd) == -1)
        throw std::runtime_error("readOutputCgi: cannot create pipe");

    pid = fork();
    if (pid == -1)
        throw std::runtime_error("readOutputCgi: fork failed");

    if (pid == 0) { // Child process
        dupAndCloseFd(pipefd);
        execl(filePath.c_str(), filePath.c_str(), (char *)nullptr);
        std::cerr << "execl failed: " << std::endl;
        _exit(EXIT_FAILURE);
    }else { // Father process
        close(pipefd[1]);
        while ((bytesRead = read(pipefd[0], buffer.data(), buffer.size())) > 0)
            result.append(buffer.data(), bytesRead);
        close(pipefd[0]);

        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            throw std::runtime_error("readOutputCgi: command failed");

        return result;
    }
}