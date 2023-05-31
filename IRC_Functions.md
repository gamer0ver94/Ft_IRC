int socket(int domain, int type, int protocol);
// Creates a new socket.
// - `domain`: Specifies the address domain of the socket (e.g., AF_INET for IPv4).
// - `type`: Specifies the type of the socket (e.g., SOCK_STREAM for TCP).
// - `protocol`: Specifies the protocol to be used by the socket (usually set to 0 to choose the default protocol).
________________________________________________________________________________________________
int close(int sockfd);
// Closes the socket.
// - `sockfd`: The file descriptor of the socket to be closed.
________________________________________________________________________________________________
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
// Sets socket options.
// - `sockfd`: The file descriptor of the socket.
// - `level`: The level at which the option is defined (e.g., SOL_SOCKET for socket-level options).
// - `optname`: The name of the option to be set.
// - `optval`: A pointer to the value to be set for the option.
// - `optlen`: The size of the `optval` buffer.
________________________________________________________________________________________________
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// Retrieves the local address and port.
// - `sockfd`: The file descriptor of the socket.
// - `addr`: A pointer to a sockaddr structure to store the retrieved address information.
// - `addrlen`: A pointer to the size of the `addr` buffer, which is updated with the actual size of the address structure.
________________________________________________________________________________________________
struct protoent *getprotobyname(const char *name);
// Retrieves protocol information.
// - `name`: The name of the protocol.
________________________________________________________________________________________________
struct hostent *gethostbyname(const char *name);
// Retrieves host information based on name.
// - `name`: The name of the host.
________________________________________________________________________________________________
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
// Resolves a hostname and service name into a list of socket addresses.
// - `node`: The hostname or IP address to be resolved.
// - `service`: The service name or port number associated with the desired socket address.
// - `hints`: A pointer to a structure providing hints or constraints for the address resolution.
// - `res`: A pointer to the resulting linked list of addrinfo structures.
________________________________________________________________________________________________
void freeaddrinfo(struct addrinfo *res);
// Frees memory allocated by getaddrinfo().
// - `res`: A pointer to the linked list of addrinfo structures to be freed.
________________________________________________________________________________________________
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// Associates a socket with a local address and port.
// - `sockfd`: The file descriptor of the socket.
// - `addr`: A pointer to a sockaddr structure specifying the local address and port.
// - `addrlen`: The size of the `addr` structure.
________________________________________________________________________________________________
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// Initiates a connection to a remote server.
// - `sockfd`: The file descriptor of the socket.
// - `addr`: A pointer to a sockaddr structure specifying the remote server's address and port.
// - `addrlen`: The size of the `addr` structure.
________________________________________________________________________________________________
int listen(int sockfd, int backlog);
// Marks a socket as passive and starts listening for incoming connections.
// - `sockfd`: The file descriptor of the socket.
// - `backlog`: The maximum number of pending connections that can be queued up.
________________________________________________________________________________________________
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// Accepts an incoming connection on a listening socket.
// - `sockfd`: The file descriptor of the listening socket.
// - `addr`: A pointer to a sockaddr structure to store the address of the connecting client.
// - `addrlen`: A pointer to the size of the `addr` buffer, which is updated with the actual size of the address structure.
________________________________________________________________________________________________
uint16_t htons(uint16_t hostshort);
// Converts a 16-bit unsigned integer from host byte order to network byte order.
// - `hostshort`: The 16-bit value in host byte order.
________________________________________________________________________________________________
uint32_t htonl(uint32_t hostlong);
// Converts a 32-bit unsigned integer from host byte order to network byte order.
// - `hostlong`: The 32-bit value in host byte order.
________________________________________________________________________________________________
uint16_t ntohs(uint16_t netshort);
// Converts a 16-bit unsigned integer from network byte order to host byte order.
// - `netshort`: The 16-bit value in network byte order.
________________________________________________________________________________________________
uint32_t ntohl(uint32_t netlong);
// Converts a 32-bit unsigned integer from network byte order to host byte order.
// - `netlong`: The 32-bit value in network byte order.
________________________________________________________________________________________________
in_addr_t inet_addr(const char *cp);
// Converts an IPv4 address from the dotted decimal format to a network byte order integer.
// - `cp`: The dotted decimal IPv4 address.
________________________________________________________________________________________________
char *inet_ntoa(struct in_addr in);
// Converts an IPv4 address from a network byte order integer to a string in dotted decimal format.
// - `in`: The IPv4 address in network byte order.
________________________________________________________________________________________________
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
// Sends data over a socket.
// - `sockfd`: The file descriptor of the socket.
// - `buf`: A pointer to the buffer containing the data to be sent.
// - `len`: The length of the data in bytes.
// - `flags`: Optional flags to control the behavior of the send operation.

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// Receives data from a socket.
// - `sockfd`: The file descriptor of the socket.
// - `buf`: A pointer to the buffer to store the received data.
// - `len`: The size of the buffer in bytes.
// - `flags`: Optional flags to control the behavior of the receive operation.
________________________________________________________________________________________________
void (*signal(int signum, void (*handler)(int)))(int);
// Sets a signal handler for a specific signal.
// - `signum`: The signal number to be handled.
// - `handler`: A pointer to the signal handler function.
________________________________________________________________________________________________
off_t lseek(int fd, off_t offset, int whence);
// Moves the file pointer associated with the file descriptor to a specified position.
// - `fd`: The file descriptor of the file.
// - `offset`: The offset in bytes from the specified `whence`.
// - `whence`: The reference position to calculate the offset (SEEK_SET, SEEK_CUR, or SEEK_END).
________________________________________________________________________________________________
int fstat(int fd, struct stat *buf);
// Retrieves information about a file.
// - `fd`: The file descriptor of the file.
// - `buf`: A pointer to the `stat` structure to store the file information.
________________________________________________________________________________________________
int fcntl(int fd, int cmd, ... /* arg */ );
// Performs various operations on a file descriptor.
// - `fd`: The file descriptor.
// - `cmd`: The command to be performed.
// - `arg`: Additional arguments depending on the command.
________________________________________________________________________________________________
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
// Monitors a set of file descriptors for events.
// - `fds`: An array of pollfd structures specifying the file descriptors and events to monitor.
// - `nfds`: The number of elements in the `fds` array.
// - `timeout`: The timeout value in milliseconds (-1 for indefinite timeout).
________________________________________________________________________________________________

